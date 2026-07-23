// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/HUD.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"
#include "TPP_HUD.generated.h"




class UTPP_BaseWidget;
class UCommonActivatableWidgetStack;
class UTPP_UIManagerRoot;

USTRUCT(BlueprintType)
struct FInitialWidgetData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag LayerTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UTPP_BaseWidget> WidgetClass;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHUDReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeSettings, int32, IndexSettings);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkipCutScene);

UCLASS()
class TPP_01_API ATPP_HUD : public AHUD
{
	
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintCallable)
	FOnChangeSettings OnChangeSettings;

	virtual void BeginPlay() override;

	void RegisterLayer(const FGameplayTag& LayerTag, UCommonActivatableWidgetContainerBase* Layer);

	UFUNCTION(BlueprintCallable)
	UTPP_BaseWidget* PushWidgetToLayer(UPARAM(meta=(Categories="UI.Layer"))const FGameplayTag LayerTag, const TSubclassOf<UTPP_BaseWidget> WidgetClass);

	UFUNCTION(BlueprintCallable)
	void PopWidgetFromLayer(UPARAM(meta=(Categories="UI.Layer"))const FGameplayTag LayerTag, UTPP_BaseWidget* Widget);

	UPROPERTY(BlueprintAssignable)
	FOnHUDReady OnHUDReady;
	UPROPERTY(BlueprintAssignable)
	FOnSkipCutScene OnSkipCutScene;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UTPP_UIManagerRoot> RootWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FInitialWidgetData> InitialWidgets;

private:
	UPROPERTY()
	TObjectPtr<UTPP_UIManagerRoot> RootWidget;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> UILayers;
};
