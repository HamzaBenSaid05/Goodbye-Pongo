#pragma once

#include "CoreMinimal.h"
#include "TPP_TriggerAIAction_Base.h"
#include "TPP_TriggerAIAction_TogglePlayerCollision.generated.h"

UCLASS()
class TPP_01_API ATPP_TriggerAIAction_TogglePlayerCollision : public ATPP_TriggerAIAction_Base
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category="Collision")
	bool bEnable;
	virtual void PongoAIAction(AActor* OtherActor = nullptr) override;
};
