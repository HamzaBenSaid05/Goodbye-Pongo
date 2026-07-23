// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Hazards/ITimerInteraction.h"
#include "Item/Hazard/FloorTraps/TPP_ElectricFloorBase.h"
#include "TPP_ElectricFloorFrequency.generated.h"

UCLASS()
class TPP_01_API ATPP_ElectricFloorFrequency : public ATPP_ElectricFloorBase,public IITimerInteraction
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void ActivationFunction() override;
	virtual void DeactivationFunction()override;
	UPROPERTY(EditAnywhere, Category="Electric Floor", meta = (DisplayPriority = "1"))
	float ActiveTime = 2.f;

	UPROPERTY(EditAnywhere, Category="Electric Floor", meta = (DisplayPriority = "1"))
	float InactiveTime = 2.f;

private:
	FTimerHandle FrequencyTimer;
	void ActivateTrap();
	void DeactivateTrap();
	void ToggleTrap();

	
	// Debug
	UPROPERTY()
	bool bIsActive;
	void DrawDebugState();
};
