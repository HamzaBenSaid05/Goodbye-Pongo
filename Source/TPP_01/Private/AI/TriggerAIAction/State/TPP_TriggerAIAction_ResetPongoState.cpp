#include "AI/TriggerAIAction/State/TPP_TriggerAIAction_ResetPongoState.h"

#include "EventProxies/TPP_AICommsProxySubsystem.h"
#include "EventProxies/AICommsEvents/EmotionalState/TPP_AIComms_ResetEvent.h"

void ATPP_TriggerAIAction_ResetPongoState::PongoAIAction(AActor* OtherActor)
{
	Super::PongoAIAction(OtherActor);
	if (UTPP_AICommsProxySubsystem* Sub = GetWorld()->GetSubsystem<UTPP_AICommsProxySubsystem>())
	{
		UTPP_AIComms_ResetEvent * ResetEvent = NewObject<UTPP_AIComms_ResetEvent>();
		Sub->NotifyAICommEvent(ResetEvent);
	}
}
