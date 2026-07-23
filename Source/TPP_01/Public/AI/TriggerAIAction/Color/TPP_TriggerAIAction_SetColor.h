#pragma once

#include "CoreMinimal.h"
#include "AI/TriggerAIAction/TPP_TriggerAIAction_Base.h"
#include "TPP_TriggerAIAction_SetColor.generated.h"

UCLASS()
class TPP_01_API ATPP_TriggerAIAction_SetColor : public ATPP_TriggerAIAction_Base
{
	GENERATED_BODY()

protected:
	virtual void PongoAIAction(AActor* OtherActor = nullptr) override;

private:
	UPROPERTY(EditAnywhere, Category="Color")
	FLinearColor Color;
	void SetColor(AActor* Actor, FLinearColor NewColor);
};
