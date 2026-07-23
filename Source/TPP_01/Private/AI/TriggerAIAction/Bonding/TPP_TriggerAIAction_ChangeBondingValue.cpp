#include "AI/TriggerAIAction/Bonding/TPP_TriggerAIAction_ChangeBondingValue.h"

#include "EventProxies/TPP_AICommsProxySubsystem.h"
#include "EventProxies/AICommsEvents/Bounding/TPP_AIComms_BondingChangeEvent.h"

void ATPP_TriggerAIAction_ChangeBondingValue::PongoAIAction(AActor* OtherActor)
{
	Super::PongoAIAction();
	if (UTPP_AICommsProxySubsystem* Sub = GetWorld()->GetSubsystem<UTPP_AICommsProxySubsystem>())
	{
		UTPP_AIComms_BondingChangeEvent* BondingChangeEvent = NewObject<UTPP_AIComms_BondingChangeEvent>();
		BondingChangeEvent->Init(BondingValueChange);
#if WITH_EDITOR
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::White, FString::Printf(TEXT("Bonding Trigger Change : %f"), BondingValueChange)); }
#endif
		Sub->NotifyAICommEvent(BondingChangeEvent);
	}
}
