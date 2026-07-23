// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/TPP_HUD.h"
#include "Components/OverlaySlot.h"
#include "UI/Widget/TPP_BaseWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Overlay.h"
#include "CommonActivatableWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/WidgetSwitcherSlot.h"
#include "UI/Widget/TPP_UIManagerRoot.h"

void ATPP_HUD::BeginPlay()
{
	Super::BeginPlay();

	if (!RootWidgetClass)
	{
		return;
	}

	RootWidget = CreateWidget<UTPP_UIManagerRoot>(GetWorld(), RootWidgetClass);
	if (!RootWidget)
	{
		return;
	}

	RootWidget->AddToViewport();

	FSlateApplication::Get().SetAllUserFocusToGameViewport();
	
	for (const FInitialWidgetData& InitialWidgetData : InitialWidgets)
	{
		PushWidgetToLayer(InitialWidgetData.LayerTag, InitialWidgetData.WidgetClass);
	}
	
}

void ATPP_HUD::RegisterLayer(const FGameplayTag& LayerTag, UCommonActivatableWidgetContainerBase* Layer)
{
	if (!IsValid(Layer))
	{
		return;
	}

	if (UILayers.Contains(LayerTag))
	{
		return;
	}

	UILayers.Add(LayerTag, Layer);
}

UTPP_BaseWidget* ATPP_HUD::PushWidgetToLayer(const FGameplayTag LayerTag, const TSubclassOf<UTPP_BaseWidget> WidgetClass)
{
	TObjectPtr<UCommonActivatableWidgetContainerBase>* LayerPtr = UILayers.Find(LayerTag);
	if (!LayerPtr)
	{
		return nullptr;
	}

	UCommonActivatableWidgetContainerBase* Layer = LayerPtr->Get();
	if (!IsValid(Layer))
	{
		return nullptr;
	}

	UTPP_BaseWidget* NewWidget = Layer->AddWidget<UTPP_BaseWidget>(WidgetClass);
	
	int32 NumWidgets = Layer->GetNumWidgets();
	UE_LOG(LogTemp, Warning, TEXT("Layer %s ora ha %d widget"), *LayerTag.ToString(), NumWidgets);
	if (!IsValid(NewWidget))
	{
		return nullptr;
	}

	return NewWidget;
}


void ATPP_HUD::PopWidgetFromLayer(const FGameplayTag LayerTag, UTPP_BaseWidget* Widget)
{
	TObjectPtr<UCommonActivatableWidgetContainerBase>* LayerPtr = UILayers.Find(LayerTag);
	if (!LayerPtr)
	{
		return;
	}

	UCommonActivatableWidgetContainerBase* Layer = LayerPtr->Get();
	if (!IsValid(Layer))
	{
		return;
	}

	if (!IsValid(Widget)) 
	{
		return;
	}
	
	Layer->RemoveWidget(*Widget);
}

