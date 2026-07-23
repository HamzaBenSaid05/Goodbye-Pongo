#include "AI/TriggerAIAction/State/TPP_TriggerAIAction_SetPongoState.h"

#include "EventProxies/TPP_AICommsProxySubsystem.h"
#include "EventProxies/AICommsEvents/EmotionalState/TPP_AIComms_ChangeEmotionalStateEvent.h"

void ATPP_TriggerAIAction_SetPongoState::PongoAIAction(AActor* OtherActor )
{
	Super::PongoAIAction(OtherActor);
	if (UTPP_AICommsProxySubsystem* Sub = GetWorld()->GetSubsystem<UTPP_AICommsProxySubsystem>())
	{
		UTPP_AIComms_ChangeEmotionalStateEvent* ChangeEmotionalStateEvent = NewObject<UTPP_AIComms_ChangeEmotionalStateEvent>();
		ChangeEmotionalStateEvent->Init(NewPongoState, EmotionalStateObject);
		Sub->NotifyAICommEvent(ChangeEmotionalStateEvent);
	}
}
