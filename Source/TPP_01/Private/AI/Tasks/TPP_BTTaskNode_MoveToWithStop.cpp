
#include "AI/Tasks/TPP_BTTaskNode_MoveToWithStop.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UTPP_BTTaskNode_MoveToWithStop::UTPP_BTTaskNode_MoveToWithStop()
{
	NodeName = "MoveToWithStop";
}

EBTNodeResult::Type UTPP_BTTaskNode_MoveToWithStop::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp))
	{
		return EBTNodeResult::Failed;
	}
	CachedBlackboard->SetValueAsBool(bCanBeStopped.SelectedKeyName, true);
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UTPP_BTTaskNode_MoveToWithStop::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                    EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	if (!CacheObjects(OwnerComp))
	{
		return;
	}

	CachedBlackboard->SetValueAsBool(bCanBeStopped.SelectedKeyName, false);
}
