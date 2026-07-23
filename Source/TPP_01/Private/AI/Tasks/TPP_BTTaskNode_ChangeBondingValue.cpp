#include "AI/Tasks/TPP_BTTaskNode_ChangeBondingValue.h"

#include "EventProxies/TPP_AICommsProxySubsystem.h"
#include "EventProxies/AICommsEvents/Bounding/TPP_AIComms_BondingChangeEvent.h"

UTPP_BTTaskNode_ChangeBondingValue::UTPP_BTTaskNode_ChangeBondingValue()
{
	NodeName = "Change Bonding Value";
}

EBTNodeResult::Type UTPP_BTTaskNode_ChangeBondingValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
                                                                    uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp))
	{
		return EBTNodeResult::Failed;
	}
	if (UTPP_AICommsProxySubsystem* Sub = GetWorld()->GetSubsystem<UTPP_AICommsProxySubsystem>())
	{
		UTPP_AIComms_BondingChangeEvent* BondingChangeEvent = NewObject<UTPP_AIComms_BondingChangeEvent>();
		BondingChangeEvent->Init(BondingValueChange);
#if WITH_EDITOR
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::White, FString::Printf(TEXT("Bonding Task Change : %f"), BondingValueChange)); }
#endif
		Sub->NotifyAICommEvent(BondingChangeEvent);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
