#include "AI/Tasks/TPP_BTTaskNode_TryInteract.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Item/Interactable/TPP_InteractableItem.h"

EBTNodeResult::Type UTPP_BTTaskNode_TryInteract::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!AIController || !Pawn || !BlackboardComp)
		return EBTNodeResult::Failed;
	AActor* EmotionalStateObject = Cast<AActor>(BlackboardComp->GetValueAsObject(EmotionalStateObjectKey.SelectedKeyName));
	if (ATPP_InteractableItem* InteractableItem = Cast<ATPP_InteractableItem>(EmotionalStateObject))
	{
		//InteractableItem->InteractWith();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Succeeded;
}
