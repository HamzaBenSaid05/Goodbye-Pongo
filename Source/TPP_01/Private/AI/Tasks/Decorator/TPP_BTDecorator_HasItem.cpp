#include "AI/Tasks/Decorator/TPP_BTDecorator_HasItem.h"

#include "Interfaces/Interact/TPP_Interactor.h"

UTPP_BTDecorator_HasItem::UTPP_BTDecorator_HasItem()
{
	NodeName = "Has Item";
}

bool UTPP_BTDecorator_HasItem::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!Super::CalculateRawConditionValue(OwnerComp, NodeMemory))
		return false;
	if (ITPP_Interactor* Interactor = Cast<ITPP_Interactor>(CachedPawn.Get()))
	{
		return Interactor->IsInteracting();
	}
	return false;
}
