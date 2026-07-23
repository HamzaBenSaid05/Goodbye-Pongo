// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ITimerInteraction.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UITimerInteraction : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TPP_01_API IITimerInteraction
{
	GENERATED_BODY()

public:
	virtual void ActivationFunction() = 0;
	virtual void DeactivationFunction() = 0;
	
	
};
