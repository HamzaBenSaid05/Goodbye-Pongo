#include "AI/Tasks/TPP_BTTaskNode_ResetColorAndSpeed.h"

#include "Interfaces/AI/TPP_AIAction.h"

UTPP_BTTaskNode_ResetColorAndSpeed::UTPP_BTTaskNode_ResetColorAndSpeed()
{
	NodeName = "ResetColorAndSpeed";
}

EBTNodeResult::Type UTPP_BTTaskNode_ResetColorAndSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
											 "Resetting Color and Speed");
		}
#endif
		AIAction->ResetColor();
		AIAction->SetSpeedBasedOnState(static_cast<uint8>(ETPP_PongoState::Idle));
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
