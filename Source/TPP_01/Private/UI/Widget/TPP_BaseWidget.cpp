// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/TPP_BaseWidget.h"
#include "CommonUI/Private/Input/UIActionRouterTypes.h"
#include "Input/CommonUIInputTypes.h"
#include "Editor/WidgetCompilerLog.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "DHL_Common_BaseWidget"



TOptional<FUIInputConfig>  UTPP_BaseWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case ETPPWidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case ETPPWidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case ETPPWidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	case ETPPWidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}
}

void  UTPP_BaseWidget::NativeOnActivated()
{	
	Super::NativeOnActivated();
	
	if (InputConfig == ETPPWidgetInputMode::Menu)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			PC->SetInputMode(FInputModeUIOnly());
			PC->bShowMouseCursor = true;
		}
	}
}

void  UTPP_BaseWidget::NativeOnDeactivated()
{	
	Super::NativeOnDeactivated();
	
	if (InputConfig == ETPPWidgetInputMode::Menu)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
		}
		
		FSlateApplication::Get().SetAllUserFocusToGameViewport();
	}

	
}

#if WITH_EDITOR

void  UTPP_BaseWidget::ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledWidgetTree(BlueprintWidgetTree, CompileLog);

	if (!GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UTPP_BaseWidget, BP_GetDesiredFocusTarget)))
	{
		if (GetParentNativeClass(GetClass()) ==  UTPP_BaseWidget::StaticClass())
		{
			CompileLog.Warning(LOCTEXT("ValidateGetDesiredFocusTarget_Warning", "GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen."));
		}
		else
		{
			//TODO - Note for now, because we can't guarantee it isn't implemented in a native subclass of this one.
			CompileLog.Note(LOCTEXT("ValidateGetDesiredFocusTarget_Note", "GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen.  If it was implemented in the native base class you can ignore this message."));
		}
	}
}

#endif

#undef LOCTEXT_NAMESPACE
