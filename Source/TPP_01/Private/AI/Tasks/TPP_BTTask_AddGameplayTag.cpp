#include "AI/Tasks/TPP_BTTask_AddGameplayTag.h"

#include "Interfaces/AI/TPP_AITaggable.h"

EBTNodeResult::Type UTPP_BTTask_AddGameplayTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp)) { return EBTNodeResult::Failed; }
	if (CachedPongo->Implements<UTPP_AITaggable>())
	{
		ITPP_AITaggable::Execute_AddGameplayTag(CachedPongo.Get(), GameplayTagToAdd);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
