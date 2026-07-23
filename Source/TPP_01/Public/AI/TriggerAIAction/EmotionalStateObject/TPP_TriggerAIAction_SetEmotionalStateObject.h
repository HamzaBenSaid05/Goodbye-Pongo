#pragma once

#include "CoreMinimal.h"
#include "AI/TriggerAIAction/TPP_TriggerAIAction_Base.h"
#include "TPP_TriggerAIAction_SetEmotionalStateObject.generated.h"

UCLASS()
class TPP_01_API ATPP_TriggerAIAction_SetEmotionalStateObject : public ATPP_TriggerAIAction_Base
{
	GENERATED_BODY()

protected:
	virtual void PongoAIAction(AActor* OtherActor = nullptr) override;

private:
	UPROPERTY(EditAnywhere, Category="Scripted|Emotional State")
	AActor* EmotionalStateObject;
	void SetEmotionalState(AActor* Actor, AActor* NewEmotionalStateObject);
};
