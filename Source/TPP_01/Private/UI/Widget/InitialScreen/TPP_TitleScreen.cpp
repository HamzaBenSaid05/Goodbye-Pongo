#include "UI/Widget/InitialScreen/TPP_TitleScreen.h"

#include "CommonActivatableWidgetSwitcher.h"
#include "Gameplay/GameInstances/TPP_GameInstance.h"
#include "Input/Reply.h"
#include "UI/HUD/TPP_HUD.h"

void UTPP_TitleScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	SetFocus();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer
		(
			TitleScreenTimerHandle,
			this,
			&UTPP_TitleScreen::SwitchControllerPage,
			DelaySeconds,
			false
		);
	}
}

FReply UTPP_TitleScreen::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bCanChangeScreen)
	{
		Switcher->SetActiveWidgetIndex(CurrentScreenIndex);
		if (CurrentScreenIndex >= 2)
		{
			LaunchMainMenu();
		}
		else
		{
			CurrentScreenIndex = (CurrentScreenIndex + 1);
		}

	}

	return FReply::Handled();
}

FReply UTPP_TitleScreen::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	if (bCanChangeScreen)
	{
		Switcher->SetActiveWidgetIndex(CurrentScreenIndex);
		if (CurrentScreenIndex >= 2)
		{
			LaunchMainMenu();
		}
		else
		{
			CurrentScreenIndex = (CurrentScreenIndex + 1);
		}

	}

	return FReply::Handled();
}

void UTPP_TitleScreen::SwitchControllerPage()
{
	CurrentScreenIndex = (CurrentScreenIndex + 1);
	Switcher->SetActiveWidgetIndex(CurrentScreenIndex);
}

void UTPP_TitleScreen::LaunchMainMenu_Implementation()
{
	UTPP_GameInstance* GameInstanceRef = Cast<UTPP_GameInstance>(GetGameInstance());
	APlayerController* PC = GetOwningPlayer();

	if (GameInstanceRef && PC)
	{
		ATPP_HUD* HudRef = Cast<ATPP_HUD>(PC->GetHUD());
		if (HudRef)
		{
			if (GameInstanceRef->GetRoomIndexToLoad() <= 0)
			{
				HudRef->PushWidgetToLayer(FGameplayTag::RequestGameplayTag(FName("UI.Layer.Menu")), Widgets[1]);
			}
			else
			{
				HudRef->PushWidgetToLayer(FGameplayTag::RequestGameplayTag(FName("UI.Layer.Menu")), Widgets[0]);
			}

			HudRef->PopWidgetFromLayer(FGameplayTag::RequestGameplayTag(FName("UI.Layer.Menu")), this);
		}

	}
}
