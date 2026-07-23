// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/TPP_BaseWidget.h"
#include "UI/HUD/TPP_HUDGame.h"
#include "TPP_TutorialWidget.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_TutorialWidget : public UTPP_BaseWidget
{
	GENERATED_BODY()
	
public:
	void SetCurrentTutorialData(FTutorialData* Data);
	void InizializeTorialWidget();
protected:	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnInputChanged(ECommonInputType NewInputType);
	UFUNCTION()
	void UpdateTutorialImage();
	
	struct FTutorialData* CurrentTutorialData;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* TutorialImage;
	
	UCommonInputSubsystem* CachedInputSubsystem;
};
