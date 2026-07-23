#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Hazards/NeedPowerSwitch.h"
#include "TPP_TriggerableCameraShake.generated.h"

UCLASS()
class TPP_01_API ATPP_TriggerableCameraShake : public AActor, public INeedPowerSwitch
{
	GENERATED_BODY()
	
public:
	virtual void SwitchPower_Implementation() override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadOnly)
	float ShakeDuration;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Shake")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;
};
