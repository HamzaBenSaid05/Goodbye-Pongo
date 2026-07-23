// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TPP_AnimatedVegetationPawn.generated.h"

UCLASS()
class TPP_01_API ATPP_AnimatedVegetationPawn : public APawn
{
	GENERATED_BODY()

	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
