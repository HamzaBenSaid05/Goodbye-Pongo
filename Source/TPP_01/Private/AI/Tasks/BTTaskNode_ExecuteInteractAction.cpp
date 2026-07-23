#include "AI/Tasks/BTTaskNode_ExecuteInteractAction.h"

#include "AIController.h"
#include "AI/AIController/TPP_PongoController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Pongo/TPP_Pongo.h"
#include "DataAsset/InteractActions/TPP_DragAction_DA.h"
#include "Item/Interactable/TPP_InteractableItem.h"
#include "DrawDebugHelpers.h"
#include "Characters/Player/TPP_PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

DEFINE_LOG_CATEGORY(LogExecuteInteraction);

UTPP_BTTaskNode_ExecuteInteractAction::UTPP_BTTaskNode_ExecuteInteractAction()
{
	NodeName = TEXT("Execute Action");
	bNotifyTick = true;
}

void UTPP_BTTaskNode_ExecuteInteractAction::CleanupInteraction(
	UBehaviorTreeComponent& OwnerComp,
	const FBTExecuteInteractMemory* Memory,
	bool bClearItemKey
) const
{
	if (!Memory) { return; }

	if (CachedPongo.Get())
	{
		if (CachedPongo->GetIsDragging()) { CachedPongo->EndDragAction(); }
		else { CachedPongo->Interact(Memory->Item->InteractWith()); }
		CachedPongo->MoveIgnoreActorRemove(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (CachedPongo->Implements<UTPP_AIAction>())
		{
			ITPP_AIAction::Execute_TogglePlayerCollision(CachedPongo.Get(), true);
		}
	}

	if (UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsBool(IsInteractingKey.SelectedKeyName, false);

		if (bClearItemKey) { BlackboardComponent->ClearValue(ItemToInteractKey.SelectedKeyName); }
	}
}

EBTNodeResult::Type UTPP_BTTaskNode_ExecuteInteractAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp)) { return EBTNodeResult::Failed; }

	
	FBTExecuteInteractMemory* Memory = reinterpret_cast<FBTExecuteInteractMemory*>(NodeMemory);
	if (!Memory) { return EBTNodeResult::Failed; }

	Memory->Item = nullptr;
	Memory->DragDestination = FVector::ZeroVector;
	Memory->FinalDestination = FVector::ZeroVector;

	Memory->Item = Cast<ATPP_InteractableItem>(
	                                           CachedBlackboard->GetValueAsObject(ItemToInteractKey.SelectedKeyName)
	                                          );

	if (!IsValid(Memory->Item))
	{
		CachedBlackboard->SetValueAsBool(IsInteractingKey.SelectedKeyName, false);
		CachedBlackboard->ClearValue(ItemToInteractKey.SelectedKeyName);
		return EBTNodeResult::Failed;
	}


	if (Memory->Item->GetInteractData()->GetObjectInteractionType() != EObjectInteractionType::Large)
	{
		CleanupInteraction(OwnerComp, Memory, true);
		return EBTNodeResult::Succeeded;
	}

	CachedPongo = Cast<ATPP_Pongo>(CachedPongo.Get());
	
	CachedPongo->Execute_TogglePlayerCollision(CachedPongo.Get(), false);
	CachedPongo->Interact(Memory->Item->InteractWith());

	CachedPongo->bCanPongoInteract = true;

	if (!IsValid(Memory->Item))
	{
		CleanupInteraction(OwnerComp, Memory, true);
		return EBTNodeResult::Failed;
	}

	Memory->FinalDestination = Memory->Item->GetDirectionToDrag();
	Memory->CachedDragDir = (Memory->FinalDestination - Memory->Item->GetActorLocation()).GetSafeNormal();

	UE_LOG(LogExecuteInteraction, Warning, TEXT("Drag FinalDestination: %s"), *Memory->FinalDestination.ToString());
	// Ignore collision with player during drag
	if (ATPP_PlayerCharacter* Player = Cast<ATPP_PlayerCharacter>(
	                                                              UGameplayStatics::GetPlayerPawn(GetWorld(), 0))) { CachedPongo->MoveIgnoreActorAdd(Player); }
	if (CachedPongo->Implements<UTPP_AIAction>())
	{
		ITPP_AIAction::Execute_TogglePlayerCollision(CachedPongo.Get(), false);
	}
	return EBTNodeResult::InProgress;
}

void UTPP_BTTaskNode_ExecuteInteractAction::TickTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTExecuteInteractMemory* Memory = reinterpret_cast<FBTExecuteInteractMemory*>(NodeMemory);
	if (!Memory)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (!IsValid(Memory->Item) || !IsValid(CachedPongo.Get()) || !IsValid(CachedPongoController.Get()))
	{
		CleanupInteraction(OwnerComp, Memory, true);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	CachedPongo = Cast<ATPP_Pongo>(CachedPongo.Get());

	CachedPongo->Execute_TogglePlayerCollision(CachedPongo.Get(), false);

#if WITH_EDITOR
	DrawDebugSphere(CachedPongo.Get()->GetWorld(), Memory->FinalDestination, 15.f, 12,
	                FColor::Red, false, 0.1f, 0, 1.f);
#endif

	// Trace to detect player in drag path
	const FVector DragDir = Memory->CachedDragDir;
	const FVector MeshExtent = Memory->Item->GetMesh()->Bounds.GetBox().GetExtent();
	const FVector ItemCenter = Memory->Item->GetMesh()->Bounds.Origin;

	const float FrontEdge = FVector::DotProduct(MeshExtent.GetAbs(), DragDir.GetAbs());
	const FVector TraceStart = ItemCenter + DragDir * (FrontEdge + TraceStartOffset);
	const FVector TraceEnd = TraceStart + DragDir * TraceLength;
	const FQuat TraceQuat = DragDir.ToOrientationQuat();
	const FVector HalfSize = FVector(30.f, 40.f, 90.f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CachedPongo.Get());
	Params.AddIgnoredActor(Memory->Item);

	const bool bHit = GetWorld()->SweepSingleByChannel(
	                                                   HitResult,
	                                                   TraceStart,
	                                                   TraceEnd,
	                                                   TraceQuat,
	                                                   ECC_Pawn,
	                                                   FCollisionShape::MakeBox(MeshExtent),
	                                                   Params
	                                                  );
#if WITH_EDITOR
	const FColor Color = bHit ? FColor::Red : FColor::Green;
	DrawDebugBox(GetWorld(), TraceStart, HalfSize, TraceQuat, Color, false, 0.1f, 0);
	DrawDebugBox(GetWorld(), TraceEnd, HalfSize, TraceQuat, Color, false, 0.1f, 0);
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Color, false, 0.1f, 0, 2.f);
#endif

	// Check if hit actor is player
	bool bPlayerDetected = false;
	if (bHit && IsValid(HitResult.GetActor()) && HitResult.GetActor()->ActorHasTag("Player"))
	{
		bPlayerDetected = true;
#if WITH_EDITOR
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, TEXT("Player Detected in Drag Path")); }
#endif
	}

	// If player detected, stop movement and wait until path is clear
	if (bPlayerDetected)
	{
		if (!Memory->bIsWaitingForPlayer)
		{
			Memory->bIsWaitingForPlayer = true;
			CachedPongoController->StopMovement();
#if WITH_EDITOR
			if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, TEXT("Player blocking -> PAUSE")); }
#endif
		}
		return;
	}

	// If was waiting for player, resume movement
	if (Memory->bIsWaitingForPlayer)
	{
		Memory->bIsWaitingForPlayer = false;
#if WITH_EDITOR
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, TEXT("Player cleared -> RESUME")); }
#endif
	}

	CachedPongoController->MovePongoToDestination(Memory->FinalDestination);

	const float Distance = FVector::Dist(CachedPongo->GetActorLocation(), Memory->FinalDestination);

	// If close enough to final destination, end interaction
	if (Distance <= DistanceToEnd)
	{
		CleanupInteraction(OwnerComp, Memory, true);
		Memory->Item->EndAIInteraction();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UTPP_BTTaskNode_ExecuteInteractAction::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogExecuteInteraction, Warning, TEXT("AbortTask called "));

	FBTExecuteInteractMemory* Memory = reinterpret_cast<FBTExecuteInteractMemory*>(NodeMemory);
	if (!Memory)
	{
		UE_LOG(LogExecuteInteraction, Warning, TEXT("AbortTask: Memory is null -> Aborted"));
		return EBTNodeResult::Aborted;
	}

	CleanupInteraction(OwnerComp, Memory, true);
	return EBTNodeResult::Aborted;
}

uint16 UTPP_BTTaskNode_ExecuteInteractAction::GetInstanceMemorySize() const { return sizeof(FBTExecuteInteractMemory); }
