// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/PongoIndicator/TPP_IndicatorFeedback.h"

#include "Components/Image.h"

void UTPP_IndicatorFeedback::SetAllignBasedOnAngle(float Angle)
{
	
}
void UTPP_IndicatorFeedback::SetSizeRatio(FVector2D Ratio)
{
	ImageIndicator->SetRenderScale(Ratio);
}

void UTPP_IndicatorFeedback::PlayWidgetAnimation_Implementation() {}
