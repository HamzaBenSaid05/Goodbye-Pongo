// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/TPP_UIManagerRoot.h"

#include "GameplayTagContainer.h"
#include "UI/HUD/TPP_HUD.h"

void UTPP_UIManagerRoot::RegisterLayer(const FGameplayTag LayerTag,UCommonActivatableWidgetContainerBase* Panel)
{
	ATPP_HUD* HUD = GetOwningPlayer()->GetHUD<ATPP_HUD>();
	if (!IsValid(HUD))
	{
		return;
	}

	HUD->RegisterLayer(LayerTag, Panel);
}
/*void UTPP_UIManagerRoot::NativeConstruct()
{
	Super::NativeConstruct();

	ATPP_HUD* HUD = GetOwningPlayer()->GetHUD<ATPP_HUD>();
	if (!HUD) return;

	HUD->RegisterLayer(FGameplayTag::RequestGameplayTag("UI.Layer.Game"), GameLayer, 0);
	HUD->RegisterLayer(FGameplayTag::RequestGameplayTag("UI.Layer.GameMenu"), GameMenuLayer, 1);
	HUD->RegisterLayer(FGameplayTag::RequestGameplayTag("UI.Layer.Menu"), MenuLayer, 2);
	HUD->RegisterLayer(FGameplayTag::RequestGameplayTag("UI.Layer.Modal"), ModalLayer, 3);
	HUD->RegisterLayer(FGameplayTag::RequestGameplayTag("UI.Layer.Overlay"), OverlayLayer, 4);
}*/
