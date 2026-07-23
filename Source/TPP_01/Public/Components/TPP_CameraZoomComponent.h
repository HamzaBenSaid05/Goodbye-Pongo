#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPP_CameraZoomComponent.generated.h"

class ATPP_Camera;
class UCurveFloat;

UENUM(BlueprintType)
enum class EZoomState : uint8
{
	Idle,
	ZoomingIn,
	Waiting,
	ZoomingOut
};

UENUM(BlueprintType)
enum class EZoomTrigger : uint8
{
	OnZoomInStart,
	OnZoomInFinished,
	OnZoomOutStart,
	OnZoomOutFinished
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraZoom, EZoomTrigger, State);

UCLASS(Blueprintable, ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class TPP_01_API UTPP_CameraZoomComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTPP_CameraZoomComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	UPROPERTY()
	TObjectPtr<ATPP_Camera> Camera;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	bool bShouldBlockPlayerInput = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	EZoomTrigger InputResumeTrigger =
		EZoomTrigger::OnZoomOutFinished;

	float Zoom = 1000.f;

	float ZoomInDuration = 5.f;

	float ZoomOutDuration = 5.f;

	float DurationBetweenTransition = 5.f;

	bool bShouldWaitBeforeZoomOut = true;

	TObjectPtr<UCurveFloat> ZoomInCurve;

	TObjectPtr<UCurveFloat> ZoomOutCurve;

	float ElapsedTime = 0.f;

	float StartZoomValue = 0.f;
	float TargetZoomValue = 0.f;
	float OriginalZoomValue = 0.f;

	EZoomState CurrentState = EZoomState::Idle;

public:
	UPROPERTY(BlueprintAssignable, Category="Camera")
	FOnCameraZoom OnCameraZoom;

	UFUNCTION(BlueprintCallable)
	void StartZoom(
		const float InZoom,
		const float InZoomDuration,
		const float InZoomOutDuration,
		const float InDurationBetweenTransition,
		const bool bInShouldWaitBeforeZoomOut,
		UCurveFloat* InZoomInCurve,
		UCurveFloat* InZoomOutCurve);

	void OnZoomInStart();

	void OnZoomInFinished();

	void OnZoomOutStart();

	void OnZoomOutFinished();

	float GetCurrentZoom() const;
	void SetZoom(float NewZoom) const;

	void SetPlayerInputEnabled(bool bEnabled) const;
};
