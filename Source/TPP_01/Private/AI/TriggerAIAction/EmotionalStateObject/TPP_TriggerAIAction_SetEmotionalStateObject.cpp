#include "AI/TriggerAIAction/EmotionalStateObject/TPP_TriggerAIAction_SetEmotionalStateObject.h"

#include "AI/AIController/TPP_PongoController.h"
#include "Characters/Pongo/TPP_Pongo.h"
#include "Gameplay/GameModes/TPP_GameMode.h"
#include "Kismet/GameplayStatics.h"

void ATPP_TriggerAIAction_SetEmotionalStateObject::PongoAIAction(AActor* OtherActor)
{
	Super::PongoAIAction(OtherActor);
	if (!OtherActor->ActorHasTag("Pongo"))
	{
		if (const ATPP_GameMode* GameMode =
			Cast<ATPP_GameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			if (ATPP_Pongo* Pongo = GameMode->GetPongoRef())
			{
				SetEmotionalState(Pongo, EmotionalStateObject);
			}
		}
	}
	SetEmotionalState(OtherActor, EmotionalStateObject);
}

void ATPP_TriggerAIAction_SetEmotionalStateObject::SetEmotionalState(AActor* Actor, AActor* NewEmotionalStateObject)
{
	if (!IsValid(Actor))
	{
		return;
	}
	if (Actor->GetClass()->ImplementsInterface(
			UTPP_AIEmotionalStateObject::StaticClass()))
	{
		ITPP_AIEmotionalStateObject::Execute_SetEmotionalStateObject(
			Actor,
			NewEmotionalStateObject);
		return;
	}

	if (AController* Controller = Actor->GetInstigatorController())
	{
		if (Controller->GetClass()->ImplementsInterface(
			UTPP_AIEmotionalStateObject::StaticClass()))
		{
			ITPP_AIEmotionalStateObject::Execute_SetEmotionalStateObject(
				Controller,
				NewEmotionalStateObject);
		}
	}
}

