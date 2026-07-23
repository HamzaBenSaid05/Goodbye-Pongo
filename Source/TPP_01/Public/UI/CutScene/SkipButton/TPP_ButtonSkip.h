// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/TPP_BaseButton.h"
#include "TPP_ButtonSkip.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_ButtonSkip : public UTPP_BaseButton
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(meta = (BindWidget))
	UCommonActionWidget* ActionWidget;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hold Progress")
	float MaxHoldTime = 2.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hold Progress")
	float ReleaseTime = 0.5f;

public:

	UFUNCTION(BlueprintCallable, Category = "Hold Progress")
	void StartHoldProgress();

	UFUNCTION(BlueprintCallable, Category = "Hold Progress")
	void StartReleaseProgress();

	void AdvanceHold();
	void AdvanceRelease();
	void UpdateMaterialPercentage(float NewPercentage);

	float CurrentPercentage = 0.0f;
	const float TimerFrequency = 0.01f;

	FTimerHandle HoldTimerHandle;
	FTimerHandle ReleaseTimerHandle;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

};
