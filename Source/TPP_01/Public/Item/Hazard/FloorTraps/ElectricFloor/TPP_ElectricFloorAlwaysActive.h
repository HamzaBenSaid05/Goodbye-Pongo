// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Hazard/FloorTraps/TPP_ElectricFloorBase.h"
#include "TPP_ElectricFloorAlwaysActive.generated.h"

UCLASS()
class TPP_01_API ATPP_ElectricFloorAlwaysActive : public ATPP_ElectricFloorBase
{
	GENERATED_BODY()
	
	void DrawDebugState();
	virtual void Tick(float DeltaTime) override;
};
