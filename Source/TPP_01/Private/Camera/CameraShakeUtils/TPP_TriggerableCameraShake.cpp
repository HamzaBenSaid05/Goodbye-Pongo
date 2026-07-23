#include "Camera/CameraShakeUtils/TPP_TriggerableCameraShake.h"

void ATPP_TriggerableCameraShake::SwitchPower_Implementation() {}

void ATPP_TriggerableCameraShake::BeginPlay()
{
	Super::BeginPlay();

	if (CameraShakeClass && CameraShakeClass.GetDefaultObject()->GetCameraShakeDuration().GetDurationType() == ECameraShakeDurationType::Fixed)
	{
		ShakeDuration = CameraShakeClass.GetDefaultObject()->GetCameraShakeDuration().Get();
	}
}
