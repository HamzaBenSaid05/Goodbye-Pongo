// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPP_ItemSplineUtility.generated.h"

UCLASS()
class TPP_01_API ATPP_ItemSplineUtility : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ATPP_ItemSplineUtility();

	UPROPERTY(EditAnywhere)
	class USplineComponent* SplineComponent;
};
