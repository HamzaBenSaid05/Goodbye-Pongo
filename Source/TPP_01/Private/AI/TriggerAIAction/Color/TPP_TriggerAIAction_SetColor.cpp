#include "AI/TriggerAIAction/Color/TPP_TriggerAIAction_SetColor.h"

#include "Interfaces/AI/TPP_AIAction.h"

void ATPP_TriggerAIAction_SetColor::PongoAIAction(AActor* OtherActor)
{
	Super::PongoAIAction(OtherActor);
	if (!IsValid(OtherActor) || !OtherActor->ActorHasTag("Pongo"))
	{
		return;
	}

	SetColor(OtherActor, Color);
}

void ATPP_TriggerAIAction_SetColor::SetColor(AActor* Actor, FLinearColor NewColor)
{
	if (!IsValid(Actor))
	{
		return;
	}
	if (Actor->Implements<UTPP_AIAction>())
	{
		ITPP_AIAction::Execute_SetColor(Actor, NewColor);
	}
}
