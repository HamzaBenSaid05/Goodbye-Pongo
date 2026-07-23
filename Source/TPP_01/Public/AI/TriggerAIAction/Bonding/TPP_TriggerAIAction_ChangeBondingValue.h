#pragma once

#include "CoreMinimal.h"
#include "AI/TriggerAIAction/TPP_TriggerAIAction_Base.h"
#include "TPP_TriggerAIAction_ChangeBondingValue.generated.h"

UCLASS()
class TPP_01_API ATPP_TriggerAIAction_ChangeBondingValue : public ATPP_TriggerAIAction_Base
{
	GENERATED_BODY()

protected:
	void virtual PongoAIAction(AActor* OtherActor = nullptr);

private:
	UPROPERTY(EditAnywhere, Category="Bonding")
	float BondingValueChange;
};
