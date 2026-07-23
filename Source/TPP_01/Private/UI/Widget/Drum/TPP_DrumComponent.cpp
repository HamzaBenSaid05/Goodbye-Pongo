// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/Drum/TPP_DrumComponent.h"

#include "Components/Image.h"

void UTPP_DrumComponent::NativeConstruct()
{
	Super::NativeConstruct();
	if (ImageIcon && ImageIcon->GetBrush().GetResourceObject())
	{
		DynamicMaterial = ImageIcon->GetDynamicMaterial();
	}
}

void UTPP_DrumComponent::SetImageTextures(class UTexture2D* Image)
{
	ImageIcon->SetBrushFromTexture(Image);
}

void UTPP_DrumComponent::SetFrameMaterialFromSpreadSheet(float Frame)
{
	DynamicMaterial->SetScalarParameterValue("Anim Value", Frame);	
}

class UImage* UTPP_DrumComponent::GetImageTextures()
{
	return ImageIcon;
}

