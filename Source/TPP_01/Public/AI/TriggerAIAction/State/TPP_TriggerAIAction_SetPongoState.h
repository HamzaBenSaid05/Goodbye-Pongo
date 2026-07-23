#pragma once

#include "CoreMinimal.h"
#include "AI/Enum/TPP_PongoEnum.h"
#include "AI/TriggerAIAction/TPP_TriggerAIAction_Base.h"
#include "TPP_TriggerAIAction_SetPongoState.generated.h"

UCLASS()
class TPP_01_API ATPP_TriggerAIAction_SetPongoState : public ATPP_TriggerAIAction_Base
{
	GENERATED_BODY()

protected:
	void virtual PongoAIAction(AActor* OtherActor = nullptr) override;
	UPROPERTY(EditAnywhere, Category="Scripted|Emotional State")
	ETPP_PongoState NewPongoState;
	UPROPERTY(EditAnywhere, Category="Scripted|Emotional State", meta=(EditCondition="NewPongoState != ETPP_PongoState::Idle", EditConditionHides))
	AActor* EmotionalStateObject;
};
