#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Hazards/NeedPowerSwitch.h"
#include "Interfaces/Hazards/ITimerInteraction.h"
#include "Item/Hazard/Statue/TPP_BaseStatueHazard.h"
#include "TPP_StillStatueHazard.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API ATPP_StillStatueHazard : public ATPP_BaseStatueHazard, public IITimerInteraction, public INeedPowerSwitch
{
	GENERATED_BODY()

public:
	// ITimerInteractionMethods 
	virtual void ActivationFunction() override;
	virtual void DeactivationFunction() override;
	// End ITimerInteractionMethods 

	// INeedPowerSwitch
	virtual void SwitchPower_Implementation() override;
	//End INeedPowerSwitch
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="EnergyType", meta = (DisplayPriority = "1"))
	bool bIsActive;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TimerData", meta = (DisplayPriority = "1"))
	float ActiveDurationTime = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TimerData", meta = (DisplayPriority = "1"))
	float DeactiveDurationTime = 4.f;
	UPROPERTY()
	float ElapseTime;
	UPROPERTY()
	FTimerHandle ActivityTimer;
	UPROPERTY()
	FTimerHandle DeactivationTimer;
};
