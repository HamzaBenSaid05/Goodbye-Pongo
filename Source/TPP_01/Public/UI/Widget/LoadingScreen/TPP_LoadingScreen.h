// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/TPP_BaseWidget.h"
#include "TPP_LoadingScreen.generated.h"


enum class ELoadingState : uint8
{
	Oscillating,
	ReturningToZero,
	ScalingUp,
	ReadyToClose,
	Closed
};
/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_LoadingScreen : public UTPP_BaseWidget
{
	GENERATED_BODY()

protected:
		
	UPROPERTY(meta = (BindWidget))
	class UImage* LoadingScreenImage;
	UPROPERTY(meta = (BindWidget))
	class UImage* BackgroundImage;
	UPROPERTY(EditAnywhere,Category = "Anim Duration")
	float MinTimeLoadingScreen;

	bool bIsPlayerSpawned = false;

	//void CheckIfPlayerSpawned();

	UPROPERTY(EditAnywhere, Category = "Loading Animation")
	float OscillationSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Loading Animation")
	float MaxRotationAngle = 15.0f;

	float RunningTime = 0.0f;

	//void UpdateLoadingAnimation();

	FTimerHandle AnimationTimerHandle;

	FTimerHandle LoadingScreenTimerHandle;


	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	ELoadingState CurrentState = ELoadingState::Oscillating;

	UPROPERTY(EditAnywhere, Category = "Loading Animation|Outro")
	float ReturnToZeroSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Loading Animation|Outro")
	float MaxScaleTarget = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Loading Animation|Outro")
	float ScaleDuration = 0.5f;

public:
	
	void InitializeLoadingScreen();
	void CloseLoadingScreen();
	void SetPlayerSpawned(bool bSpawned);

private:
	float LastFrameTime = 0.0f;
	float ScaleElapsed = 0.0f;
	FVector2D InitialScale = FVector2D(1.0f, 1.0f);
	
	float RealTimeElapsed = 0.0f;
};
