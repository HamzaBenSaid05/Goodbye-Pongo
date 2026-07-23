// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TPP_InteractAction_DA.generated.h"

UENUM(Blueprintable)
enum class EObjectInteractionType : uint8
{
	Small,
	Medium,
	Large,
	None,
	ElectricSwitch,
	CuriosityObject,
	Pedestal,
	Orb,
	MockDrum,
};
/**
 * 
 */
UCLASS(Blueprintable)
class TPP_01_API UTPP_InteractAction_DA : public UDataAsset
{
	GENERATED_BODY()
	
	public:
	UPROPERTY(EditAnywhere)
	EObjectInteractionType InteractionType;
	
	FORCEINLINE EObjectInteractionType GetObjectInteractionType() const { return InteractionType; }
};
