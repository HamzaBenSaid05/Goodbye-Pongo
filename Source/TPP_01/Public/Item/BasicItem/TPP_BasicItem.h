// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Hazards/NeedPowerSwitch.h"
#include "TPP_BasicItem.generated.h"

UCLASS()
class TPP_01_API ATPP_BasicItem : public AActor,public INeedPowerSwitch
{
	GENERATED_BODY()

public:
	virtual void SwitchPower_Implementation() override;
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayAnimation();
	
	UPROPERTY(BlueprintReadWrite)
	bool bIsActive;
	
};
