// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPP_DrumManager.h"
#include "UI/Widget/TPP_BaseWidget.h"
#include "TPP_DrumComponent.generated.h"


/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_DrumComponent : public UTPP_BaseWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* ImageIcon;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;
public:
	void SetImageTextures(class UTexture2D* Image);
	class UImage* GetImageTextures();
	
	void SetFrameMaterialFromSpreadSheet(float Frame);
	
};
