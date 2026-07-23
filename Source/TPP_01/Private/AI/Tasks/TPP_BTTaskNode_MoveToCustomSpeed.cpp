#include "AI/Tasks/TPP_BTTaskNode_MoveToCustomSpeed.h"
#include "Interfaces/AI/TPP_AIAction.h"

UTPP_BTTaskNode_MoveToCustomSpeed::UTPP_BTTaskNode_MoveToCustomSpeed()
{
	NodeName = "Move To Custom Speed";
}

EBTNodeResult::Type UTPP_BTTaskNode_MoveToCustomSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp))
	{
		return EBTNodeResult::Failed;
	}
	if (ITPP_AIAction* AIAction = Cast<ITPP_AIAction>(CachedPawn.Get()))
	{
		CachedBlackboard->SetValueAsBool(bIsMovingKey.SelectedKeyName, true);     
		AIAction->SetSpeedBasedOnState(CachedBlackboard->GetValueAsEnum(PongoStateKey.SelectedKeyName));
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UTPP_BTTaskNode_MoveToCustomSpeed::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
	if (CachedPawn->Implements<UTPP_AIAction>())
	{
		CachedBlackboard->SetValueAsBool(bIsMovingKey.SelectedKeyName, false);
		ITPP_AIAction::Execute_TogglePlayerCollision(CachedPawn.Get(), true);
	}
}
