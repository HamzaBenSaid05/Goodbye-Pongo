// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/TPP_BaseWidget.h"
#include "TPP_IndicatorFeedback.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_IndicatorFeedback : public UTPP_BaseWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UOverlay* OverlaySlot;
	UPROPERTY(meta = (BindWidget))
	class UImage* ImageIndicator;
	
	public:
	void SetAllignBasedOnAngle(float Angle);
	void SetSizeRatio(FVector2D Ratio);
	
	UFUNCTION(BlueprintNativeEvent, Category="Tutorial")
	void PlayWidgetAnimation();
	void PlayWidgetAnimation_Implementation();
};
