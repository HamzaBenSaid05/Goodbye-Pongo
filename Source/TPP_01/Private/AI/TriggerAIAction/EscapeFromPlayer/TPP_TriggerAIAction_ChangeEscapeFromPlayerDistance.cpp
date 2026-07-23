#include "AI/TriggerAIAction/EscapeFromPlayer/TPP_TriggerAIAction_ChangeEscapeFromPlayerDistance.h"

#include "EventProxies/TPP_AICommsProxySubsystem.h"
#include "EventProxies/AICommsEvents/FearFromPlayer/TPP_AIComms_ChangeEscapeFromPlayerDistance.h"

void ATPP_TriggerAIAction_ChangeEscapeFromPlayerDistance::PongoAIAction(AActor* OtherActor)
{
	Super::PongoAIAction();
	if (UTPP_AICommsProxySubsystem* Sub = GetWorld()->GetSubsystem<UTPP_AICommsProxySubsystem>())
	{
		UTPP_AIComms_ChangeEscapeFromPlayerDistance* ChangeEscapeFromPlayerDistanceEvent = NewObject<UTPP_AIComms_ChangeEscapeFromPlayerDistance>();
		ChangeEscapeFromPlayerDistanceEvent->Init(EscapeFromPlayerDistanceMaxValue,EscapeFromPlayerDistanceMinValue);
#if WITH_EDITOR
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::White, FString::Printf(TEXT("Escape From Player Distance Change : %f max, %f min"), EscapeFromPlayerDistanceMaxValue, EscapeFromPlayerDistanceMinValue)); }
#endif
		Sub->NotifyAICommEvent(ChangeEscapeFromPlayerDistanceEvent);
	}
}
