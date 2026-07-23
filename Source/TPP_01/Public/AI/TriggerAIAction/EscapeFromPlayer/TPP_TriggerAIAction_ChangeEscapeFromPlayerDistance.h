#pragma once

#include "CoreMinimal.h"
#include "AI/TriggerAIAction/TPP_TriggerAIAction_Base.h"
#include "TPP_TriggerAIAction_ChangeEscapeFromPlayerDistance.generated.h"

UCLASS()
class TPP_01_API ATPP_TriggerAIAction_ChangeEscapeFromPlayerDistance : public ATPP_TriggerAIAction_Base
{
	GENERATED_BODY()

protected:
	virtual void PongoAIAction(AActor* OtherActor) override;

private:
	UPROPERTY(EditAnywhere, Category="Fear From Player")
	float EscapeFromPlayerDistanceMaxValue;
	UPROPERTY(EditAnywhere, Category="Fear From Player")
	float EscapeFromPlayerDistanceMinValue;
};
