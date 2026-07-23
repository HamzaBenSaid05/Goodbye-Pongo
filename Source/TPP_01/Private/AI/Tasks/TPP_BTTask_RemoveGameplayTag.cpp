#include "AI/Tasks/TPP_BTTask_RemoveGameplayTag.h"

#include "Interfaces/AI/TPP_AITaggable.h"

EBTNodeResult::Type UTPP_BTTask_RemoveGameplayTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp)) { return EBTNodeResult::Failed; }
	if (CachedPongo->Implements<UTPP_AITaggable>())
	{
		ITPP_AITaggable::Execute_RemoveGameplayTag(CachedPongo.Get(), GameplayTagToRemove);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
