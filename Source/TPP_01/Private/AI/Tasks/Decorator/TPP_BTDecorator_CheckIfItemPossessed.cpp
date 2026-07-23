#include "AI/Tasks/Decorator/TPP_BTDecorator_CheckIfItemPossessed.h"

#include "Item/Interactable/TPP_InteractableItem.h"

UTPP_BTDecorator_CheckIfItemPossessed::UTPP_BTDecorator_CheckIfItemPossessed()
{
	NodeName = "Check If Item Possessed";
}

bool UTPP_BTDecorator_CheckIfItemPossessed::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!CacheObjects(OwnerComp))
	{
		return EBTNodeResult::Failed;
	}
	
	ATPP_InteractableItem* Item = Cast<ATPP_InteractableItem>(CachedBlackboard->GetValueAsObject(ItemKey.SelectedKeyName));
	
	if (!Item)
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Item Null"));
		}
#endif
		return EBTNodeResult::Failed;
	}
	
	
	if (Item->InteractWith() != nullptr)
	{
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
