#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TPP_TriggerAIAction_SetPongoState.h"
#include "TPP_TriggerAIAction_SetScriptedPongoState.generated.h"

UCLASS()
class TPP_01_API ATPP_TriggerAIAction_SetScriptedPongoState : public ATPP_TriggerAIAction_SetPongoState
{
	GENERATED_BODY()
	
protected:
	void virtual PongoAIAction(AActor* OtherActor = nullptr) override;

private:
	UPROPERTY(EditAnywhere, Category="Scripted|Emotional State")
	FGameplayTag NewScriptedStateTag;
};