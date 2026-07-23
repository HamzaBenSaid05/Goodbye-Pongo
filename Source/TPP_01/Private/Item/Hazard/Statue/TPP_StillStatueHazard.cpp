#include "Item/Hazard/Statue/TPP_StillStatueHazard.h"

void ATPP_StillStatueHazard::BeginPlay()
{
	Super::BeginPlay();

	if (bIsActive)
	{
		InitializeStatueAnim();
		GetWorld()->GetTimerManager().SetTimer(
		                                       ActivityTimer,
		                                       this,
		                                       &ATPP_StillStatueHazard::ActivationFunction,
		                                       0.1f,
		                                       true
		                                      );
	}
}

void ATPP_StillStatueHazard::ActivationFunction()
{
	bIsActive = true;
	//ScanForCharacters();

	if (DeactiveDurationTime <= 0)
		return;

	ElapseTime += 0.1f;

	if (ElapseTime >= ActiveDurationTime)
	{
		DeinitializeStatueAnim();
		GetWorld()->GetTimerManager().ClearTimer(ActivityTimer);
		ElapseTime = 0;
		GetWorld()->GetTimerManager().SetTimer(DeactivationTimer,
		                                       this,
		                                       &ATPP_StillStatueHazard::DeactivationFunction,
		                                       0.1f,
		                                       true);
	}
}

void ATPP_StillStatueHazard::DeactivationFunction()
{
	if (LightCone->IsActive())
	{
		bIsActive = false;
	}
	ElapseTime += 0.1f;

	if (ElapseTime >= DeactiveDurationTime)
	{
		InitializeStatueAnim();
		GetWorld()->GetTimerManager().ClearTimer(DeactivationTimer);
		ElapseTime = 0;
		GetWorld()->GetTimerManager().SetTimer(ActivityTimer,
		                                       this,
		                                       &ATPP_StillStatueHazard::ActivationFunction,
		                                       0.1f,
		                                       true);
	}
}

void ATPP_StillStatueHazard::SwitchPower_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(ActivityTimer);
	GetWorld()->GetTimerManager().ClearTimer(DeactivationTimer);
	ElapseTime = 0.0f;

	if (bIsActive)
	{
		bIsActive = false;
		DeinitializeStatueAnim();
	}
	else
	{
		bIsActive = true;
		InitializeStatueAnim();
		GetWorld()->GetTimerManager().SetTimer(
		                                       ActivityTimer,
		                                       this,
		                                       &ATPP_StillStatueHazard::ActivationFunction,
		                                       0.1f,
		                                       true
		                                      );
	}
}
