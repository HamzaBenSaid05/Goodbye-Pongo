#include "Components/TPP_CameraZoomComponent.h"

#include "Camera/TPP_Camera.h"
#include "GameFramework/SpringArmComponent.h"

UTPP_CameraZoomComponent::UTPP_CameraZoomComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTPP_CameraZoomComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Camera = Cast<ATPP_Camera>(GetOwner());

	if (!Camera)
	{
		UE_LOG(LogTemp, Error,
			TEXT("CameraZoomComponent can only be attached to ATPP_Camera"));
		return;
	}

	PlayerController = GetWorld()->GetFirstPlayerController();
	
}

void UTPP_CameraZoomComponent::OnZoomInStart()
{
	OriginalZoomValue = GetCurrentZoom();
	if (bShouldBlockPlayerInput && InputResumeTrigger == EZoomTrigger::OnZoomInStart)
	{
		SetPlayerInputEnabled(false);
	}
	OnCameraZoom.Broadcast(EZoomTrigger::OnZoomInStart);
}

void UTPP_CameraZoomComponent::OnZoomInFinished()
{
	if (bShouldBlockPlayerInput && InputResumeTrigger == EZoomTrigger::OnZoomInFinished)
	{
		SetPlayerInputEnabled(true);
	}
	OnCameraZoom.Broadcast(EZoomTrigger::OnZoomInFinished);
}

void UTPP_CameraZoomComponent::OnZoomOutStart()
{
	if (bShouldBlockPlayerInput && InputResumeTrigger == EZoomTrigger::OnZoomOutStart)
	{
		SetPlayerInputEnabled(true);
	}
	OnCameraZoom.Broadcast(EZoomTrigger::OnZoomOutStart);
}

void UTPP_CameraZoomComponent::OnZoomOutFinished()
{
	if (bShouldBlockPlayerInput && InputResumeTrigger == EZoomTrigger::OnZoomOutFinished)
	{
		SetPlayerInputEnabled(true);
	}
	OnCameraZoom.Broadcast(EZoomTrigger::OnZoomOutFinished);
}

void UTPP_CameraZoomComponent::StartZoom(const float InZoom,
	const float InZoomDuration,
	const float InZoomOutDuration,
	const float InDurationBetweenTransition,
	const bool bInShouldWaitBeforeZoomOut,
	UCurveFloat* InZoomInCurve,
	UCurveFloat* InZoomOutCurve)
{
	if (!Camera) return;

	if (bShouldBlockPlayerInput)
		SetPlayerInputEnabled(false);
	
	Zoom = InZoom;
	ZoomInDuration = InZoomDuration;
	ZoomOutDuration = InZoomOutDuration;
	DurationBetweenTransition = InDurationBetweenTransition;
	bShouldWaitBeforeZoomOut = bInShouldWaitBeforeZoomOut;
	ZoomInCurve = InZoomInCurve;
	ZoomOutCurve = InZoomOutCurve;

	StartZoomValue = GetCurrentZoom();
	TargetZoomValue = Zoom;

	ElapsedTime = 0.f;
	CurrentState = EZoomState::ZoomingIn;

	OnZoomInStart();
}

float UTPP_CameraZoomComponent::GetCurrentZoom() const
{
	if (!Camera)
		return 0.f;

	return Camera->GetSpringArmComponent()->TargetArmLength;
}

void UTPP_CameraZoomComponent::SetZoom(float NewZoom) const
{
	if (!Camera)
		return;

	Camera->GetSpringArmComponent()->TargetArmLength = NewZoom;
}

void UTPP_CameraZoomComponent::SetPlayerInputEnabled(bool bEnabled) const
{
	if (PlayerController.Get() == nullptr) return;
	if (bEnabled)
	{
		PlayerController->EnableInput(PlayerController.Get());
	}
	else
	{
		PlayerController->DisableInput(PlayerController.Get());
	}
}

void UTPP_CameraZoomComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(
		DeltaTime,
		TickType,
		ThisTickFunction);
	if (CurrentState == EZoomState::Idle || !Camera) return;

	ElapsedTime += DeltaTime;

	switch (CurrentState)
	{
	case EZoomState::ZoomingIn:
		{
			float Alpha = FMath::Clamp(ElapsedTime / ZoomInDuration, 0.f, 1.f);

			if (ZoomInCurve)
				Alpha = ZoomInCurve->GetFloatValue(Alpha);

			float NewZoom = FMath::Lerp(StartZoomValue, TargetZoomValue, Alpha);
			SetZoom(NewZoom);

			if (ElapsedTime >= ZoomInDuration)
			{
				OnZoomInFinished();

				if (bShouldWaitBeforeZoomOut)
				{
					CurrentState = EZoomState::Waiting;
					ElapsedTime = 0.f;
					
				}
				else
				{
					CurrentState = EZoomState::ZoomingOut;
					ElapsedTime = 0.f;

					StartZoomValue = GetCurrentZoom();
					TargetZoomValue = 0.f;

					OnZoomOutStart();
				}
			}
			break;
		}

	case EZoomState::Waiting:
		{

			if (ElapsedTime >= DurationBetweenTransition)
			{
				CurrentState = EZoomState::ZoomingOut;
				ElapsedTime = 0.f;

				StartZoomValue = GetCurrentZoom();
				TargetZoomValue = OriginalZoomValue;

				OnZoomOutStart();
			}
			break;
		}

	case EZoomState::ZoomingOut:
		{
			float Alpha = FMath::Clamp(ElapsedTime / ZoomOutDuration, 0.f, 1.f);

			if (ZoomOutCurve)
				Alpha = ZoomOutCurve->GetFloatValue(Alpha);

			float NewZoom = FMath::Lerp(StartZoomValue, TargetZoomValue, Alpha);
			SetZoom(NewZoom);

			if (ElapsedTime >= ZoomOutDuration)
			{
				OnZoomOutFinished();
				CurrentState = EZoomState::Idle;
			}
			break;
		}
	case EZoomState::Idle:
		break;
	}
}
