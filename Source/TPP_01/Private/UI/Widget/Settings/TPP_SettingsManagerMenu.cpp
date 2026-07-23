// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/Settings/TPP_SettingsManagerMenu.h"

#include "UI/HUD/TPP_HUD.h"
#include "Input/CommonUIInputTypes.h"
#include "Components/InputComponent.h"
#include "CommonUI/Private/Input/UIActionRouterTypes.h"

void UTPP_SettingsManagerMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	Switcher->SetActiveWidgetIndex(CurrentSettingIndex);

}

void UTPP_SettingsManagerMenu::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	FBindUIActionArgs NextArgs(NextTabActionRow, FSimpleDelegate::CreateUObject(this, &UTPP_SettingsManagerMenu::HandleNextTab));
	NextArgs.bDisplayInActionBar = true;
		
	FUIActionBindingHandle HandleBindF = RegisterUIActionBinding(NextArgs);
	BindingHandles.Add(HandleBindF);
	
	FBindUIActionArgs PrevArgs(PrevTabActionRow, FSimpleDelegate::CreateUObject(this, &UTPP_SettingsManagerMenu::HandlePreviousTab));
	PrevArgs.bDisplayInActionBar = true;

	FUIActionBindingHandle HandleBindP = RegisterUIActionBinding(PrevArgs);
	BindingHandles.Add(HandleBindP);

	APlayerController* PC = GetOwningPlayer();

	if (!PC) return;
	ATPP_HUD* HudRef = Cast<ATPP_HUD>(PC->GetHUD());
	if (!HudRef) return;

	HudRef->OnChangeSettings.AddDynamic(this, &ThisClass::OnChangeSettinAtIndex);
	
}

void UTPP_SettingsManagerMenu::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	for (FUIActionBindingHandle& Handle : BindingHandles)
	{
		Handle.Unregister();
	}
	BindingHandles.Empty();
}

void UTPP_SettingsManagerMenu::HandleNextTab_Implementation()
{
	ChangeSettingIndex(1);
}

void UTPP_SettingsManagerMenu::HandlePreviousTab_Implementation()
{
	ChangeSettingIndex(-1);
}

void UTPP_SettingsManagerMenu::ChangeSettingIndex(int32 Direction)
{
	
	CurrentSettingIndex = CurrentSettingIndex + Direction;
	
	APlayerController* PC = GetOwningPlayer();
	
	if (!PC) return;
	ATPP_HUD* HudRef = Cast<ATPP_HUD>(PC->GetHUD());
	if (!HudRef) return;
	
	if (CurrentSettingIndex < 0)
	{
		CurrentSettingIndex = 0;
		return;
	}
	
	if (CurrentSettingIndex > MaxCount)
		CurrentSettingIndex = MaxCount;
	
	Switcher->SetActiveWidgetIndex(CurrentSettingIndex);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Green,
			FString::Printf(TEXT("Switcher Index: %d"), CurrentSettingIndex)
		);
	}
	
}

void UTPP_SettingsManagerMenu::OnChangeSettinAtIndex(int32 Index)
{
	CurrentSettingIndex = Index;

	APlayerController* PC = GetOwningPlayer();

	if (!PC) return;
	ATPP_HUD* HudRef = Cast<ATPP_HUD>(PC->GetHUD());
	if (!HudRef) return;

	Switcher->SetActiveWidgetIndex(CurrentSettingIndex);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Green,
			FString::Printf(TEXT("Switcher Index: %d"), CurrentSettingIndex)
		);
	}
}

/*FReply UTPP_SettingsManagerMenu::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();


	if (Key == EKeys::Right || Key == EKeys::Gamepad_RightShoulder) 
	{
		HandleNextTab_Implementation();
		return FReply::Handled();
	}


	if (Key == EKeys::Left || Key == EKeys::Gamepad_LeftShoulder) 
	{
		HandlePreviousTab_Implementation();
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}*/

