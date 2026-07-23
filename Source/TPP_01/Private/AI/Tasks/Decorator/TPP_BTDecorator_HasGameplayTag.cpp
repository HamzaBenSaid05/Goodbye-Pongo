#include "AI/Tasks/Decorator/TPP_BTDecorator_HasGameplayTag.h"

#include "Interfaces/AI/TPP_AITaggable.h"

bool UTPP_BTDecorator_HasGameplayTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!Super::CalculateRawConditionValue(OwnerComp, NodeMemory))
		return false;
	if (CachedPawn->Implements<UTPP_AITaggable>()) { return ITPP_AITaggable::Execute_HasGameplayTag(CachedPawn.Get(), GameplayTagToCheck); }
	return false;
}
