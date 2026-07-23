#include "AI/TriggerAIAction/State/TPP_TriggerAIAction_SetScriptedPongoState.h"

#include "Characters/Pongo/TPP_Pongo.h"
#include "Gameplay/GameModes/TPP_GameMode.h"
#include "Interfaces/AI/TPP_AITaggable.h"

void ATPP_TriggerAIAction_SetScriptedPongoState::PongoAIAction(AActor* OtherActor)
{
	Super::PongoAIAction(OtherActor);
	if (OtherActor->Implements<UTPP_AITaggable>())
	{
		ITPP_AITaggable::Execute_AddGameplayTag(OtherActor, NewScriptedStateTag);
		return;
	}
	if (ATPP_GameMode* GameMode = Cast<ATPP_GameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (GameMode->GetPongoRef()->Implements<UTPP_AITaggable>())
		{
			ITPP_AITaggable::Execute_AddGameplayTag(GameMode->GetPongoRef(), NewScriptedStateTag);
		}
		return;
	}
	
}
