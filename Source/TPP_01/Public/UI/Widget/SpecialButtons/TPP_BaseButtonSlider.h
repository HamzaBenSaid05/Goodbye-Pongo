// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/TPP_BaseButton.h"
#include "TPP_BaseButtonSlider.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_BaseButtonSlider : public UTPP_BaseButton
{
	GENERATED_BODY()
	
	

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class USlider> AnalogSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider Settings")
	float SliderStepSize = 0.05f;

	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	

	void AdjustSliderValue(float StepDirection);

	static bool bLastInputWasGamepad;
};
