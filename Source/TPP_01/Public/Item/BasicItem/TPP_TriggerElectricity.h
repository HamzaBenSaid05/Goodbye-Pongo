// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Hazards/IPowerSwitch.h"
#include "TPP_TriggerElectricity.generated.h"

UCLASS()
class TPP_01_API ATPP_TriggerElectricity : public AActor,public IIPowerSwitch
{
	GENERATED_BODY()
	
public:	
	ATPP_TriggerElectricity();
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;
	
	UFUNCTION()
	void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);

	UFUNCTION()
	virtual void SwitchElectricity_Implementation() override;
	UFUNCTION()
	virtual void SetHazardManager(class ATPP_ElectricityManager* ElectricityManager) override;
	
	class ATPP_ElectricityManager* ElectricityManagerRef;
};
