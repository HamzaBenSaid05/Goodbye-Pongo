#include "Components/TPP_PetBar.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

void UTPP_PetBar::BeginPlay()
{
	Super::BeginPlay();
	ReassuredMin = ReassuredData.GetMinValueOfRange(MaxPetBarValue);
	ReassuredMax = ReassuredData.GetMaxValueOfRange(MaxPetBarValue);
	UnconcernedMin = UnconcernedData.GetMinValueOfRange(MaxPetBarValue);
	UnconcernedMax = UnconcernedData.GetMaxValueOfRange(MaxPetBarValue);
	AnnoyedMin = AnnoyedData.GetMinValueOfRange(MaxPetBarValue);
	AnnoyedMax = AnnoyedData.GetMaxValueOfRange(MaxPetBarValue);
}

void UTPP_PetBar::UpdatePetState()
{
	EPetStatus NewStatus = EPetStatus::None;
	float SpeedMultiplier = 1.f;

	if (CurrentPetBarValue >= ReassuredMin && CurrentPetBarValue <= ReassuredMax)
	{
		NewStatus = EPetStatus::Reassured;
		SpeedMultiplier = ReassuredData.SpeedMultiplier;
#if WITH_EDITOR
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Pet Status: Reassured")); }
#endif
	}
	else if (CurrentPetBarValue >= UnconcernedMin && CurrentPetBarValue <= UnconcernedMax)
	{
#if WITH_EDITOR
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Pet Status: Unconcerned")); }
#endif
		NewStatus = EPetStatus::Unconcerned;
		SpeedMultiplier = UnconcernedData.SpeedMultiplier;
	}
	else if (CurrentPetBarValue >= AnnoyedMin && CurrentPetBarValue <= AnnoyedMax)
	{
#if WITH_EDITOR
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Pet Status: Annoyed")); }
#endif
		NewStatus = EPetStatus::Annoyed;
		SpeedMultiplier = AnnoyedData.SpeedMultiplier;
	}

	CurrentPetStatus = NewStatus;

#if WITH_EDITOR
	if (GEngine)
	{
		FString StatusString = TEXT("None");

		switch (CurrentPetStatus)
		{
		case EPetStatus::Reassured:
			StatusString = TEXT("Reassured");
			break;
		case EPetStatus::Unconcerned:
			StatusString = TEXT("Unconcerned");
			break;
		case EPetStatus::Annoyed:
			StatusString = TEXT("Annoyed");
			break;
		default:
			break;
		}

		GEngine->AddOnScreenDebugMessage(
		                                 -1,
		                                 2.f,
		                                 FColor::Green,
		                                 FString::Printf(TEXT("Pet Status: %s"), *StatusString)
		                                );
	}
#endif

	OnPetChangedState.Broadcast(CurrentPetStatus);
	OnPetChangedSpeedMultiplier.Broadcast(SpeedMultiplier);
}

void UTPP_PetBar::StartCustomTimer(
	FTimerHandle& InTimerHandle,
	const FTimerDelegate& Delegate,
	float InTimerRate,
	bool bInLoop,
	float InFirstDelay)
{
	if (!GetWorld()) { return; }
	
	GetWorld()->GetTimerManager().SetTimer(
	                                       InTimerHandle,
	                                       Delegate,
	                                       InTimerRate,
	                                       bInLoop,
	                                       InFirstDelay
	                                      );
}

void UTPP_PetBar::ChangePetBarValue(float ChangeValue)
{
	CurrentPetBarValue = FMath::Clamp(
	                                  CurrentPetBarValue + ChangeValue,
	                                  MinPetBarValue,
	                                  MaxPetBarValue
	                                 );

#if WITH_EDITOR
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
		                                 -1,
		                                 2.f,
		                                 FColor::Blue,
		                                 FString::Printf(TEXT("Pet Bar Value: %f"), CurrentPetBarValue)
		                                );
	}
#endif
}

void UTPP_PetBar::AddPetValue(float ChangeValue)
{
	ChangePetBarValue(ChangeValue);
	UpdatePetState();

	if (!GetWorld()) { return; }

	GetWorld()->GetTimerManager().ClearTimer(PetBarTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(SpeedMultiplierTimerHandle);
	StartCustomTimer(PetBarTimerHandle,
	                 FTimerDelegate::CreateUObject(this, &UTPP_PetBar::HandleDecreaseTick),
	                 DecreaseEachTime,
	                 true,
	                 DecreaseEachTime
	                );
	StartCustomTimer(SpeedMultiplierTimerHandle,
	                 FTimerDelegate::CreateLambda([this]()
	                 {
	                 	if (GEngine)
	                 	{
							 GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
															  FString::Printf(TEXT("Reset Speed Multiplier")));
						 }
		                 OnPetChangedSpeedMultiplier.Broadcast(1.f);
	                 }),
	                 TimerSpeedMultiplier,
	                 false,
	                 TimerSpeedMultiplier
	                );
}

void UTPP_PetBar::HandleDecreaseTick()
{
	ChangePetBarValue(-DecreasePercent);

	if (CurrentPetBarValue <= MinPetBarValue) { if (GetWorld()) { GetWorld()->GetTimerManager().ClearTimer(PetBarTimerHandle); } }
}
