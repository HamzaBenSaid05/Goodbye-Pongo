#include "AI/TriggerAIAction/TPP_TriggerAIAction_TogglePlayerCollision.h"

#include "Interfaces/AI/TPP_AIAction.h"

void ATPP_TriggerAIAction_TogglePlayerCollision::PongoAIAction(AActor* OtherActor)
{
	Super::PongoAIAction(OtherActor);
	if (OtherActor->Implements<UTPP_AIAction>())
	{
		ITPP_AIAction::Execute_TogglePlayerCollision(OtherActor, bEnable);
	}
}
