#include "UI/CutScene/TPP_CutSceneWidget.h"
#include "CutScene/TPP_LevelSequencePlayer.h"
#include "UI/CutScene/TPP_Comic_Base.h"
#include "UI/HUD/TPP_HUDGame.h"
#include <Kismet/GameplayStatics.h>
#include "Components/Image.h"
#include "CommonActionWidget.h"
#include "TimerManager.h"

void UTPP_CutSceneWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
	if (ActionWidget)
	{
		ActionWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ActionWidget->SetRenderOpacity(1.0f);
	}
	if (ProgressImage)
	{
		ProgressImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ProgressImage->SetRenderOpacity(1.0f);
	}

	CurrentOpacity = 1.0f;
	TargetOpacity = 1.0f;

	GetWorld()->GetTimerManager().SetTimer(InactivityTimerHandle, this, &UTPP_CutSceneWidget::HideActionPrompt, InactivityHideTime, false);

	ATPP_HUDGame* Hud = Cast<ATPP_HUDGame>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	if (Hud)
	{
		Hud->OnSkipCutScene.AddDynamic(this, &UTPP_CutSceneWidget::SkipCutScene);
	}
}

void UTPP_CutSceneWidget::InizializeCutSceneWidget(TSubclassOf<class UTPP_Comic_Base> ComicToPlay)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC == nullptr) return;

	if (ATPP_HUDGame* GameHud = Cast<ATPP_HUDGame>(PC->GetHUD()))
	{
		ComicPlayed = Cast<UTPP_Comic_Base>(GameHud->PushWidgetToLayer(FGameplayTag::RequestGameplayTag(FName("UI.Layer.GameMenu")), ComicToPlay));
	}
}

void UTPP_CutSceneWidget::DeactivateCutSceneWidget()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC == nullptr) return;

	if (ATPP_HUDGame* GameHud = Cast<ATPP_HUDGame>(PC->GetHUD()))
	{
		GameHud->PopWidgetFromLayer(FGameplayTag::RequestGameplayTag(FName("UI.Layer.GameMenu")), ComicPlayed);
	}
}

void UTPP_CutSceneWidget::SkipCutScene()
{
	DeactivateCutSceneWidget();
	if (LevelSequencePlayer)
	{
		LevelSequencePlayer->StopSequence();
	}
}


FReply UTPP_CutSceneWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	ShowActionPrompt();

	FKey PressedKey = InKeyEvent.GetKey();

	if (PressedKey == EKeys::E || PressedKey == EKeys::Gamepad_FaceButton_Bottom)
	{
		if (!bIsHoldingKey)
		{
			bIsHoldingKey = true;
			GetWorld()->GetTimerManager().SetTimer(DelayHoldTimerHandle, this, &UTPP_CutSceneWidget::StartHoldProgress, HoldStartDelay, false);
		}
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UTPP_CutSceneWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey ReleasedKey = InKeyEvent.GetKey();

	if (ReleasedKey == EKeys::E || ReleasedKey == EKeys::Gamepad_FaceButton_Bottom)
	{
		if (bIsHoldingKey)
		{
			bIsHoldingKey = false;
			GetWorld()->GetTimerManager().ClearTimer(DelayHoldTimerHandle);

			StartReleaseProgress();
		}
		return FReply::Handled();
	}

	return FReply::Unhandled();
}


void UTPP_CutSceneWidget::StartHoldProgress()
{
	GetWorld()->GetTimerManager().ClearTimer(ReleaseTimerHandle);
	if (CurrentPercentage >= 1.0f) return;

	GetWorld()->GetTimerManager().SetTimer(HoldTimerHandle, this, &UTPP_CutSceneWidget::AdvanceHold, TimerFrequency, true);
}

void UTPP_CutSceneWidget::StartReleaseProgress()
{
	GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
	if (CurrentPercentage <= 0.0f) return;

	GetWorld()->GetTimerManager().SetTimer(ReleaseTimerHandle, this, &UTPP_CutSceneWidget::AdvanceRelease, TimerFrequency, true);
}

void UTPP_CutSceneWidget::AdvanceHold()
{
	if (MaxHoldTime > 0.0f)
	{
		CurrentPercentage += (TimerFrequency / MaxHoldTime);
		CurrentPercentage = FMath::Clamp(CurrentPercentage, 0.0f, 1.0f);

		UpdateMaterialPercentage(CurrentPercentage);

		if (CurrentPercentage >= 1.0f)
		{
			GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
			UpdateMaterialPercentage(0);
			SkipCutScene();
		}
	}
	else
	{
		CurrentPercentage = 1.0f;
		UpdateMaterialPercentage(CurrentPercentage);
		GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
		SkipCutScene();
	}
}

void UTPP_CutSceneWidget::AdvanceRelease()
{
	if (ReleaseTime > 0.0f)
	{
		CurrentPercentage -= (TimerFrequency / ReleaseTime);
		CurrentPercentage = FMath::Clamp(CurrentPercentage, 0.0f, 1.0f);

		UpdateMaterialPercentage(CurrentPercentage);

		if (CurrentPercentage <= 0.0f)
		{
			GetWorld()->GetTimerManager().ClearTimer(ReleaseTimerHandle);
		}
	}
	else
	{
		CurrentPercentage = 0.0f;
		UpdateMaterialPercentage(CurrentPercentage);
		GetWorld()->GetTimerManager().ClearTimer(ReleaseTimerHandle);
	}
}

void UTPP_CutSceneWidget::UpdateMaterialPercentage(float NewPercentage)
{
	if (!ProgressImage) return;

	if (!DynamicMaterial)
	{
		DynamicMaterial = ProgressImage->GetDynamicMaterial();
	}

	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(MaterialParameterName, NewPercentage);
	}
}

void UTPP_CutSceneWidget::ShowActionPrompt()
{
	if (ActionWidget && ActionWidget->GetVisibility() == ESlateVisibility::Hidden) ActionWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (ProgressImage && ProgressImage->GetVisibility() == ESlateVisibility::Hidden) ProgressImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	TargetOpacity = 1.0f;

	if (!GetWorld()->GetTimerManager().IsTimerActive(FadeTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, this, &UTPP_CutSceneWidget::AdvanceFade, TimerFrequency, true);
	}

	GetWorld()->GetTimerManager().SetTimer(InactivityTimerHandle, this, &UTPP_CutSceneWidget::HideActionPrompt, InactivityHideTime, false);
}

void UTPP_CutSceneWidget::HideActionPrompt()
{
	if (bIsHoldingKey) return;

	TargetOpacity = 0.0f;

	if (!GetWorld()->GetTimerManager().IsTimerActive(FadeTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, this, &UTPP_CutSceneWidget::AdvanceFade, TimerFrequency, true);
	}
}

void UTPP_CutSceneWidget::AdvanceFade()
{
	if (FadeDuration <= 0.0f)
	{
		CurrentOpacity = TargetOpacity;
	}
	else
	{
		float FadeStep = TimerFrequency / FadeDuration;

		if (CurrentOpacity < TargetOpacity)
		{
			CurrentOpacity = FMath::Min(CurrentOpacity + FadeStep, TargetOpacity);
		}
		else
		{
			CurrentOpacity = FMath::Max(CurrentOpacity - FadeStep, TargetOpacity);
		}
	}

	if (ActionWidget) ActionWidget->SetRenderOpacity(CurrentOpacity);
	if (ProgressImage) ProgressImage->SetRenderOpacity(CurrentOpacity);

	if (FMath::IsNearlyEqual(CurrentOpacity, TargetOpacity, 0.01f))
	{
		CurrentOpacity = TargetOpacity;
		GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);

		if (TargetOpacity == 0.0f)
		{
			if (ActionWidget) ActionWidget->SetVisibility(ESlateVisibility::Hidden);
			if (ProgressImage) ProgressImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}