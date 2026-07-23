// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Hazards/NeedPowerSwitch.h"
#include "TPP_HazardManager.generated.h"

USTRUCT(BlueprintType)
struct FItemToLinkPower
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TArray<AActor*> ItemTournableOn;
	
};

UCLASS()
class TPP_01_API ATPP_ElectricityManager : public AActor
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Electricity")
	TMap<AActor*, FItemToLinkPower> ManagedItems;
public:
	UFUNCTION()
	void OnSwitchActivationHandle(AActor* SwitchActor);
};
