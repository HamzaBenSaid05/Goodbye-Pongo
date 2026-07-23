// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LoadingScreen/TPP_LoadingScreen.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/TPP_HUDGame.h"
#include "Components/Image.h"
#include "GameplayTagContainer.h"

void UTPP_LoadingScreen::InitializeLoadingScreen()
{
	SetPlayerSpawned(false);
	CurrentState = ELoadingState::Oscillating;
	
	ScaleElapsed = 0.0f;
	RunningTime = 0.0f;
	RealTimeElapsed = 0.0f;

	if (LoadingScreenImage)
	{
		InitialScale = LoadingScreenImage->GetRenderTransform().Scale;
	}

}

void UTPP_LoadingScreen::SetPlayerSpawned(bool bSpawned)
{
	bIsPlayerSpawned = bSpawned;

	if (bIsPlayerSpawned && GetWorld())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}

void UTPP_LoadingScreen::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!LoadingScreenImage) return;

	
	RealTimeElapsed += InDeltaTime;

	
	if (CurrentState == ELoadingState::Oscillating)
	{
		if (bIsPlayerSpawned && RealTimeElapsed >= MinTimeLoadingScreen)
		{
			CurrentState = ELoadingState::ReturningToZero;
		}
	}

	switch (CurrentState)
	{
		case ELoadingState::Oscillating:
		{
			RunningTime += InDeltaTime * OscillationSpeed;
			float TargetAngle = FMath::Sin(RunningTime) * MaxRotationAngle;
			LoadingScreenImage->SetRenderTransformAngle(TargetAngle);
			break;
		}

		case ELoadingState::ReturningToZero:
		{
			float CurrentAngle = LoadingScreenImage->GetRenderTransformAngle();
			float NewAngle = FMath::FInterpTo(CurrentAngle, 0.0f, InDeltaTime, ReturnToZeroSpeed);
			LoadingScreenImage->SetRenderTransformAngle(NewAngle);

			if (FMath::IsNearlyEqual(NewAngle, 0.0f, 0.05f))
			{
				LoadingScreenImage->SetRenderTransformAngle(0.0f);
				CurrentState = ELoadingState::ScalingUp;
			}
			break;
		}

		case ELoadingState::ScalingUp:
		{
			if (ScaleDuration <= 0.0f)
			{
				CurrentState = ELoadingState::ReadyToClose;
				break;
			}

			ScaleElapsed += InDeltaTime;
			float Alpha = FMath::Clamp(ScaleElapsed / ScaleDuration, 0.0f, 1.0f);
			float CurrentScaleValue = FMath::Lerp(InitialScale.X, MaxScaleTarget, Alpha);
			LoadingScreenImage->SetRenderScale(FVector2D(CurrentScaleValue, CurrentScaleValue));

			if (Alpha >= 1.0f)
			{
				CurrentState = ELoadingState::ReadyToClose;
			}
			break;
		}

		case ELoadingState::ReadyToClose:
		{
			CurrentState = ELoadingState::Closed;
			CloseLoadingScreen();
			break;
		}
		
		case ELoadingState::Closed: {

			break;
		}
			
	}
}

void UTPP_LoadingScreen::CloseLoadingScreen()
{
	if (GetWorld())
	{
		
		UGameplayStatics::SetGamePaused(GetWorld(), false);

		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			ATPP_HUDGame* Hud = Cast<ATPP_HUDGame>(PC->GetHUD());
			if (Hud)
			{
				Hud->PopWidgetFromLayer(FGameplayTag::RequestGameplayTag(FName("UI.Layer.Menu")), this);
				Hud->EnableInputPlayer();
				Hud->OnHUDReady.Broadcast();
			}
		}
	}
}