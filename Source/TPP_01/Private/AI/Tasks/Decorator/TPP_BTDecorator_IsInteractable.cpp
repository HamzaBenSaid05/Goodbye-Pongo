#include "AI/Tasks/Decorator/TPP_BTDecorator_IsInteractable.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Item/Interactable/TPP_InteractableItem.h"

UTPP_BTDecorator_IsInteractable::UTPP_BTDecorator_IsInteractable() { NodeName = "Is Interactable"; }

bool UTPP_BTDecorator_IsInteractable::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                                 uint8* NodeMemory) const
{
	if (!Super::CalculateRawConditionValue(OwnerComp, NodeMemory))
		return false;

	AActor* Actor = Cast<AActor>(
		CachedBlackboard->GetValueAsObject(BlackboardKey.SelectedKeyName)
	);
	if (ATPP_InteractableItem* Interactable = Cast<ATPP_InteractableItem>(Actor))
	{
		return Interactable->GetInteractData() != nullptr;	
	}
	return false;
}
