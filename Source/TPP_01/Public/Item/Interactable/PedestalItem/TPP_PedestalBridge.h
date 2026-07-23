// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPP_PedestalItemBase.h"
#include "Interfaces/Hazards/IPowerSwitch.h"
#include "TPP_PedestalBridge.generated.h"

UCLASS()
class TPP_01_API ATPP_PedestalBridge : public ATPP_PedestalItemBase
{
	GENERATED_BODY()

public:
	ATPP_PedestalBridge();

protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintNativeEvent)
	void EnableBridge();
	UFUNCTION(BlueprintNativeEvent)
	void DisableBridge();
	virtual void EnableObject() override;
	virtual void DisableObject() override;
public:
	virtual void Tick(float DeltaTime) override;
	
};
