#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Hazards/IPowerSwitch.h"
#include "TPP_ElectricSwitch.generated.h"

UCLASS()
class TPP_01_API ATPP_ElectricSwitch : public AActor, public IIPowerSwitch
{
	GENERATED_BODY()
	
public:
	virtual void SwitchElectricity_Implementation() override;

	virtual void SetHazardManager(class ATPP_ElectricityManager* ElectricityManager) override;

protected:
	UPROPERTY()
	class ATPP_ElectricityManager* ElectricityManagerRef;
};
