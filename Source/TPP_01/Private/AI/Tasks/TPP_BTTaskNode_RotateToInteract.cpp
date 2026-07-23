#include "AI/Tasks/TPP_BTTaskNode_RotateToInteract.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Pongo/TPP_Pongo.h"
#include "UI/Widget/PongoIndicator/TPP_PongoIndicator.h"

UTPP_BTTaskNode_RotateToInteract::UTPP_BTTaskNode_RotateToInteract() { bNotifyTick = true; }

EBTNodeResult::Type UTPP_BTTaskNode_RotateToInteract::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    
	if (!ControlledPawn || !BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	ATPP_Pongo* Pongo = Cast<ATPP_Pongo>(ControlledPawn);
	if (Pongo)
	{
		Pongo->Execute_TogglePlayerCollision(Pongo, false);
		if (Pongo->GetInteractableItem())
		{
			FRotator DropRotation = Pongo->GetDropRotation();
			BlackboardComp->SetValueAsRotator(TargetRotationKey.SelectedKeyName, DropRotation);
		}
	}
	
	return EBTNodeResult::InProgress;
}

void UTPP_BTTaskNode_RotateToInteract::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	ATPP_Pongo* Pongo = Cast<ATPP_Pongo>(ControlledPawn);
	if (Pongo)
	{
		Pongo->Execute_TogglePlayerCollision(Pongo, false);
	}
	if (!ControlledPawn || !BlackboardComp)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	FRotator TargetRotation = BlackboardComp->GetValueAsRotator(TargetRotationKey.SelectedKeyName);
    
	FRotator CurrentRotation = ControlledPawn->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationSpeed);

	ControlledPawn->SetActorRotation(NewRotation);

	if (CurrentRotation.Equals(TargetRotation, 1.0f)) 
	{ 
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); 
	}
}