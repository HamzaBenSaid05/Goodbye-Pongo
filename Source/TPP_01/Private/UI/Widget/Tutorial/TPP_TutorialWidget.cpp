// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/Tutorial/TPP_TutorialWidget.h"
#include "Components/Image.h"
#include "CommonInputSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"


void UTPP_TutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CachedInputSubsystem = nullptr;

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			CachedInputSubsystem = LP->GetSubsystem<UCommonInputSubsystem>();
		}
	}

	if (CachedInputSubsystem)
	{
		CachedInputSubsystem->OnInputMethodChangedNative.AddUObject(
			this,
			&UTPP_TutorialWidget::OnInputChanged
		);
	}

	UpdateTutorialImage();
}

void UTPP_TutorialWidget::NativeDestruct()
{
	if (CachedInputSubsystem)
	{
		CachedInputSubsystem->OnInputMethodChangedNative.RemoveAll(this);
	}

	Super::NativeDestruct();
}


void UTPP_TutorialWidget::OnInputChanged(ECommonInputType NewInputType)
{
	UpdateTutorialImage();
}

void UTPP_TutorialWidget::UpdateTutorialImage()
{
	if (!CurrentTutorialData || !TutorialImage) return;

	ECommonInputType InputType = CachedInputSubsystem->GetCurrentInputType();

	UTexture2D* SelectedTexture = nullptr;

	if (InputType == ECommonInputType::Gamepad)
	{
		if (CurrentTutorialData->GamepadImages.Num() > 0)
		{
			SelectedTexture = CurrentTutorialData->GamepadImages[0];
		}
	}
	else 
	{
		if (CurrentTutorialData->KeyboardImages.Num() > 0)
		{
			SelectedTexture = CurrentTutorialData->KeyboardImages[0];
		}
	}

	if (SelectedTexture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(SelectedTexture);
		TutorialImage->SetBrush(Brush);
	}
}

void UTPP_TutorialWidget::SetCurrentTutorialData(FTutorialData* Data)
{
	CurrentTutorialData = Data;
}

void UTPP_TutorialWidget::InizializeTorialWidget()
{
	if (!CurrentTutorialData || !TutorialImage || !CachedInputSubsystem) return;

	ECommonInputType InputType = CachedInputSubsystem->GetCurrentInputType();

	UTexture2D* SelectedTexture = nullptr;

	if (InputType == ECommonInputType::Gamepad)
	{
		if (CurrentTutorialData->GamepadImages.Num() > 0)
		{
			SelectedTexture = CurrentTutorialData->GamepadImages[0];
		}
	}
	else 
	{
		if (CurrentTutorialData->KeyboardImages.Num() > 0)
		{
			SelectedTexture = CurrentTutorialData->KeyboardImages[0];
		}
	}

	if (SelectedTexture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(SelectedTexture);
		TutorialImage->SetBrush(Brush);
	}
}
