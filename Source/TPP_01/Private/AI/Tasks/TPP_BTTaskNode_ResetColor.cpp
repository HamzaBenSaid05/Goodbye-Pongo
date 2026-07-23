#include "AI/Tasks/TPP_BTTaskNode_ResetColor.h"

#include "Interfaces/AI/TPP_AIAction.h"

UTPP_BTTaskNode_ResetColor::UTPP_BTTaskNode_ResetColor()
{
	NodeName = "ResetColor";
}

EBTNodeResult::Type UTPP_BTTaskNode_ResetColor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp))
	{
		return EBTNodeResult::Failed;
	}

	if (ITPP_AIAction* AIAction = Cast<ITPP_AIAction>(CachedPongo.Get()))
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
											 "Resetting Color");
		}
#endif
		AIAction->ResetColor();
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
