// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Hazards/NeedPowerSwitch.h"
#include "Item/Hazard/FloorTraps/TPP_ElectricFloorBase.h"
#include "TPP_ElectricFloorEnergySource.generated.h"

UCLASS()
class TPP_01_API ATPP_ElectricFloorEnergySource : public ATPP_ElectricFloorBase, public INeedPowerSwitch
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, meta = (DisplayPriority = "1"))
	bool bIsActive;
	virtual void SwitchPower_Implementation() override;
	virtual void Tick(float DeltaTime) override;
	
	void DrawDebugState();
};
