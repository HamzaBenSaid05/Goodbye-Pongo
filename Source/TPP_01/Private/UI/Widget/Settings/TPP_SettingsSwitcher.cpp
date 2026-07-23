// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Settings/TPP_SettingsSwitcher.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "Components/WidgetSwitcher.h"
#include "GameFramework/GameUserSettings.h"
#include "Gameplay/GameInstances/TPP_GameInstance.h"
#include "Input/CommonUIInputTypes.h"
#include "Components/InputComponent.h"

void UTPP_SettingsSwitcher::NativeConstruct()
{
	Super::NativeConstruct();

	if (Navigation)
	{
		Navigation->Left.Rule = EUINavigationRule::Stop;
		Navigation->Right.Rule = EUINavigationRule::Stop;
	}

	GameInstanceRef = Cast<UTPP_GameInstance>(GetGameInstance());
	if (GameInstanceRef)
	{
		// FORZA IL MINIMO A 1: Se era a 0 per qualche vecchio salvataggio, lo portiamo a 1
		if (GameInstanceRef->GetCurrentGraphicsSettings() < 1)
		{
			GameInstanceRef->SetCurrentGraphicsSettings(1);
		}

		Switcher->SetActiveWidgetIndex(GameInstanceRef->GetCurrentGraphicsSettings());
		if (GEngine && GEngine->GetGameUserSettings())
		{
			UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
			if (UserSettings)
			{
				UserSettings->SetOverallScalabilityLevel(GameInstanceRef->GetCurrentGraphicsSettings());
				UserSettings->ApplyNonResolutionSettings();
			}
		}
	}
	
}
void UTPP_SettingsSwitcher::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	OnClicked().AddUObject(this, &UTPP_SettingsSwitcher::ChangeGraphicSettingsUp);
}

void UTPP_SettingsSwitcher::HandleFocusReceived()
{
	Super::HandleFocusReceived();

	FBindUIActionArgs NextArgs(NextTabActionRow, FSimpleDelegate::CreateUObject(this, &UTPP_SettingsSwitcher::ChangeGraphicSettingsUp));
	NextArgs.bDisplayInActionBar = true;
		
	FUIActionBindingHandle HandleBindF = RegisterUIActionBinding(NextArgs);
	BindingHandles.Add(HandleBindF);
	
	FBindUIActionArgs PrevArgs(PrevTabActionRow, FSimpleDelegate::CreateUObject(this, &UTPP_SettingsSwitcher::ChangeGraphicSettingsDown));
	PrevArgs.bDisplayInActionBar = true;

	FUIActionBindingHandle HandleBindP = RegisterUIActionBinding(PrevArgs);
	BindingHandles.Add(HandleBindP);
}

void UTPP_SettingsSwitcher::HandleFocusLost()
{
	Super::HandleFocusLost();

	for (FUIActionBindingHandle& Handle : BindingHandles)
	{
		Handle.Unregister();
	}
	BindingHandles.Empty();
}


void UTPP_SettingsSwitcher::ChangeGraphicSettingsUp()
{
	if (!GameInstanceRef) return;

	int32 CurrentSetting = GameInstanceRef->GetCurrentGraphicsSettings();
	int32 NextSetting = CurrentSetting + 1;

	if (NextSetting > 3)
	{
		NextSetting = 1;
	}

	GameInstanceRef->SetCurrentGraphicsSettings(NextSetting);

	if (Switcher)
	{
		Switcher->SetActiveWidgetIndex(GameInstanceRef->GetCurrentGraphicsSettings());
	}

	if (GEngine && GEngine->GetGameUserSettings())
	{
		UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
		if (UserSettings)
		{
			UserSettings->SetOverallScalabilityLevel(GameInstanceRef->GetCurrentGraphicsSettings());
			UserSettings->ApplyNonResolutionSettings();
		}
	}
}
void UTPP_SettingsSwitcher::ChangeGraphicSettingsDown()
{

	if (!GameInstanceRef) return;

	int32 CurrentSetting = GameInstanceRef->GetCurrentGraphicsSettings();
	int32 PrevSetting = CurrentSetting - 1;

	
	if (PrevSetting < 1)
	{
		PrevSetting = 3;
	}

	GameInstanceRef->SetCurrentGraphicsSettings(PrevSetting);

	if (Switcher)
	{
		Switcher->SetActiveWidgetIndex(GameInstanceRef->GetCurrentGraphicsSettings());
	}

	if (GEngine && GEngine->GetGameUserSettings())
	{
		UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
		if (UserSettings)
		{
			UserSettings->SetOverallScalabilityLevel(GameInstanceRef->GetCurrentGraphicsSettings());
			UserSettings->ApplyNonResolutionSettings();
		}
	}
}

FReply UTPP_SettingsSwitcher::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();


	if (Key == EKeys::Right || Key == EKeys::Gamepad_DPad_Right)
	{
		ChangeGraphicSettingsUp();
		return FReply::Handled();
	}


	if (Key == EKeys::Left || Key == EKeys::Gamepad_DPad_Left)
	{
		ChangeGraphicSettingsDown();
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

