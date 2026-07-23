#include "AI/Tasks/Base/TPP_BTTaskNode_MoveToBase.h"

EBTNodeResult::Type UTPP_BTTaskNode_MoveToBase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp))
	{
		return EBTNodeResult::Failed;
	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

bool UTPP_BTTaskNode_MoveToBase::CacheObjects(UBehaviorTreeComponent& OwnerComp) const
{
	CachedController = OwnerComp.GetAIOwner();
	CachedBlackboard = OwnerComp.GetBlackboardComponent();
	CachedPawn = CachedController.IsValid() ? CachedController->GetPawn() : nullptr;

	return CachedController.IsValid() && CachedPawn.IsValid() && CachedBlackboard.IsValid();
}