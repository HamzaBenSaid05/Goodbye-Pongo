#pragma once

#include "CoreMinimal.h"
#include "AI/TriggerAIAction/TPP_TriggerAIAction_Base.h"
#include "TPP_TriggerAIAction_UnlockPet.generated.h"

UCLASS()
class TPP_01_API ATPP_TriggerAIAction_UnlockPet : public ATPP_TriggerAIAction_Base
{
	GENERATED_BODY()

protected:
	void virtual PongoAIAction(AActor* OtherActor = nullptr);
};
