// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPP_InteractAction_DA.h"
#include "Enums/Item/OrbitType.h"
#include "TPP_Orb_DA.generated.h"
/**
 * 
 */
UCLASS(Blueprintable)
class TPP_01_API UTPP_Orb_DA : public UTPP_InteractAction_DA
{
	GENERATED_BODY()
	public:
	UPROPERTY(EditAnywhere)
	OrbitType ColorType;

	FORCEINLINE OrbitType GetColorType() const { return ColorType; }
};
