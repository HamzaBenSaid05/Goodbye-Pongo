#include "AI/Tasks/TPP_BTTask_EndInteract.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Pongo/TPP_Pongo.h"
#include "Item/Interactable/TPP_InteractableItem.h"
#include "DataAsset/InteractActions/TPP_DragAction_DA.h"

UTPP_BTTask_EndInteract::UTPP_BTTask_EndInteract()
{
	NodeName = "End Interact";
	bNotifyTick = true; 
	TargetRotationKey.SelectedKeyName = FName(TEXT("ItemTargetRotation"));
}

EBTNodeResult::Type UTPP_BTTask_EndInteract::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ATPP_Pongo* Pongo = Cast<ATPP_Pongo>(AIController->GetPawn());
	if (!Pongo) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	if (Pongo->GetInteractableItem() && Pongo->GetInteractableItem()->GetInteractData())
	{
		
		if (Pongo->GetInteractableItem()->GetInteractData()->GetObjectInteractionType() == EObjectInteractionType::Medium)
		{
			FRotator DropRotation = Pongo->GetDropRotation();
			BlackboardComp->SetValueAsRotator(TargetRotationKey.SelectedKeyName, DropRotation);

			return EBTNodeResult::InProgress;
		}

		Pongo->EndInteract(Pongo->GetInteractableItem()->GetInteractData());
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Succeeded;
}

void UTPP_BTTask_EndInteract::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ATPP_Pongo* Pongo = Cast<ATPP_Pongo>(AIController->GetPawn());
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!Pongo || !BlackboardComp || !Pongo->GetInteractableItem() || !Pongo->GetInteractableItem()->GetInteractData())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FRotator TargetRotation = BlackboardComp->GetValueAsRotator(TargetRotationKey.SelectedKeyName);
	FRotator CurrentRotation = Pongo->GetActorRotation();

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationSpeed);
	Pongo->SetActorRotation(NewRotation);

	if (NewRotation.Equals(TargetRotation, 1.0f))
	{
		Pongo->SetActorRotation(TargetRotation); 

		Pongo->EndInteract(Pongo->GetInteractableItem()->GetInteractData());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

