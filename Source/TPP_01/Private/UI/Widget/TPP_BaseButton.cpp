// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/TPP_BaseButton.h"

#include "CommonActionWidget.h"
#include "ICommonInputModule.h"
#include "Slate/SObjectWidget.h"

void UTPP_BaseButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ResetInputAction();
}

void UTPP_BaseButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	UpdateButtonStyle();
	RefreshButtonText();
}

void UTPP_BaseButton::HandleFocusLost()
{
	if (bShouldSelectUponReceivingFocus && GetSelected())
	{
		ClearSelection();
	}

	Super::HandleFocusLost();

	UpdateButtonStyle();
}

void UTPP_BaseButton::NativeOnSelected(bool bBroadcast)
{
	SetTriggeringInputAction(DefaultInputActionRow);
	Super::NativeOnSelected(bBroadcast);
}

void UTPP_BaseButton::NativeOnDeselected(bool bBroadcast)
{
	SetTriggeringInputAction(NullInputActionRow);
	Super::NativeOnDeselected(bBroadcast);

	UpdateButtonStyle();
}

void UTPP_BaseButton::UpdateInputActionWidget()
{
	if (bAllowInputIcon)
	{
		Super::UpdateInputActionWidget();
	}

	UpdateButtonStyle();
	RefreshButtonText();
}

void UTPP_BaseButton::OnInputMethodChanged(ECommonInputType CurrentInputType)
{
	Super::OnInputMethodChanged(CurrentInputType);

	UpdateButtonStyle();
}

FReply UTPP_BaseButton::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{

	const FKey Key = InKeyEvent.GetKey();


	if (Key == EKeys::SpaceBar)
	{
		NativeOnClicked();
		return FReply::Handled();
	}
		
	
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UTPP_BaseButton::RefreshButtonText()
{
	if (bOverrideButtonText && !ButtonText.IsEmpty())
	{
		UpdateButtonText(ButtonText);
		return;
	}

	if (InputActionWidget)
	{
		const FText ActionDisplayText = InputActionWidget->GetDisplayText();
		if (!ActionDisplayText.IsEmpty())
		{
			UpdateButtonText(ActionDisplayText);
		}
	}
}

void UTPP_BaseButton::ResetInputAction()
{
	if (bShouldUseFallbackDefaultInputAction && TriggeringInputAction.IsNull())
	{
		DefaultInputActionRow = ICommonInputModule::GetSettings().GetDefaultClickAction();
	}
	GetInputAction(DefaultInputActionRow);
	SetTriggeringInputAction(NullInputActionRow);
}

void UTPP_BaseButton::ForceRefreshStyle()
{
	if (bShouldSelectUponReceivingFocus && GetSelected())
	{
		ClearSelection();
	}

	UpdateButtonStyle();
}
