#include "AI/Tasks/Base/TPP_BTTask_WaitBase.h"

EBTNodeResult::Type UTPP_BTTask_WaitBase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp))
	{
		return EBTNodeResult::Failed;
	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

bool UTPP_BTTask_WaitBase::CacheObjects(UBehaviorTreeComponent& OwnerComp) const
{
	CachedController = OwnerComp.GetAIOwner();
	CachedBlackboard = OwnerComp.GetBlackboardComponent();
	CachedPawn = CachedController.IsValid() ? CachedController->GetPawn() : nullptr;

	return CachedController.IsValid() && CachedPawn.IsValid() && CachedBlackboard.IsValid();
}
