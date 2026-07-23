// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TPP_UIManagerRoot.generated.h"


class UCommonActivatableWidgetContainerBase;
/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_UIManagerRoot : public UUserWidget
{
	GENERATED_BODY()
	//UFUNCTION(BlueprintCallable)
	//void RegisterLayer(UPARAM( meta=(Categories="UI.Layer"))const FGameplayTag LayerTag,UOverlay* Panel,int32 LayerPriority);

public:
	UFUNCTION(BlueprintCallable)
	void RegisterLayer(UPARAM( meta=(Categories="UI.Layer"))const FGameplayTag LayerTag,UCommonActivatableWidgetContainerBase* Panel);
/*	void NativeConstruct() override;

	//Hud barra vita info
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetContainerBase> GameLayer;
// DPad segli i tipi di attacchi 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetContainerBase> GameMenuLayer;
// meni 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetContainerBase> MenuLayer;
// pop up 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetContainerBase> ModalLayer;
// sopra a tutto 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetContainerBase> OverlayLayer;*/
};
