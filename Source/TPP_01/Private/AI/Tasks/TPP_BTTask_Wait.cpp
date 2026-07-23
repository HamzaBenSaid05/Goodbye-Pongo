#include "AI/Tasks/TPP_BTTask_Wait.h"

UTPP_BTTask_Wait::UTPP_BTTask_Wait()
{
	NodeName = "Custom Wait";
}
EBTNodeResult::Type UTPP_BTTask_Wait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp))
	{
		return EBTNodeResult::Failed;
	}
	
	// Set the wait time of the base class
	WaitTime = CachedBlackboard->GetValueAsFloat(WaitTimeKey.SelectedKeyName);
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
