#include "AI/TriggerAIAction/Pet/TPP_TriggerAIAction_UnlockPet.h"

#include "Interfaces/AI/TPP_AIPettable.h"

void ATPP_TriggerAIAction_UnlockPet::PongoAIAction(AActor* OtherActor)
{
	Super::PongoAIAction();
	if (OtherActor->GetInstigatorController()->Implements<UTPP_AIPettable>())
	{
		ITPP_AIPettable::Execute_UnlockPet(OtherActor->GetInstigatorController());
	}
}
