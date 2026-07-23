#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPP_PetBar.generated.h"

USTRUCT(BlueprintType)
struct FPetBarData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Pet")
	float MinPercent = 0.f;

	UPROPERTY(EditAnywhere, Category = "Pet")
	float MaxPercent = 0.f;

	UPROPERTY(EditAnywhere, Category = "Pet")
	float SpeedMultiplier = 0.f;

	float GetMinValueOfRange(const float MaxRange) const { return (MaxRange * MinPercent) / 100.f; }

	float GetMaxValueOfRange(const float MaxRange) const { return (MaxRange * MaxPercent) / 100.f; }
};

UENUM(BlueprintType)
enum class EPetStatus : uint8
{
	None UMETA(DisplayName = "None"),
	Reassured UMETA(DisplayName = "Reassured"),
	Unconcerned UMETA(DisplayName = "Unconcerned"),
	Annoyed UMETA(DisplayName = "Annoyed"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPetChangedStateDelegate, EPetStatus, NewState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPetChangedSpeedMultiplierDelegate, float, SpeedMultiplier);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPP_01_API UTPP_PetBar : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Pet|Reassured")
	FPetBarData ReassuredData;

	UPROPERTY(EditAnywhere, Category = "Pet|Unconcerned")
	FPetBarData UnconcernedData;

	UPROPERTY(EditAnywhere, Category = "Pet|Annoyed")
	FPetBarData AnnoyedData;

	UPROPERTY(EditAnywhere, Category = "Pet|Annoyed")
	float AnnoyedTravelDistance = 0.f;

	UPROPERTY(EditAnywhere, Category = "Pet|Bar")
	float MinPetBarValue = 0.f;

	UPROPERTY(EditAnywhere, Category = "Pet|Bar")
	float MaxPetBarValue = 100.f;

	UPROPERTY(EditAnywhere, Category = "Pet|Bar")
	float DecreaseEachTime = 5.f;

	UPROPERTY(EditAnywhere, Category = "Pet|Bar")
	float TimerSpeedMultiplier = 5.f;

	// Value removed at each tick
	UPROPERTY(EditAnywhere, Category = "Pet|Bar")
	float DecreasePercent = 5.f;

	FTimerHandle PetBarTimerHandle;
	FTimerHandle SpeedMultiplierTimerHandle;

	UPROPERTY(VisibleAnywhere, Category = "Pet|Bar")
	float CurrentPetBarValue = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Pet|Bar")
	EPetStatus CurrentPetStatus = EPetStatus::None;

	float ReassuredMin;
	float ReassuredMax;
	float UnconcernedMin;
	float UnconcernedMax;
	float AnnoyedMin;
	float AnnoyedMax;
	void ChangePetBarValue(float ChangeValue);
	UFUNCTION()
	void HandleDecreaseTick();
	void UpdatePetState();
	void StartCustomTimer(
		FTimerHandle& InTimerHandle,
		const FTimerDelegate& Delegate,
		float InTimerRate,
		bool bInLoop,
		float InFirstDelay
	);

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable)
	FPetChangedStateDelegate OnPetChangedState;
	UPROPERTY(BlueprintAssignable)
	FPetChangedSpeedMultiplierDelegate OnPetChangedSpeedMultiplier;
	void AddPetValue(float ChangeValue);
	float GetAnnoyedTravelDistance() const { return AnnoyedTravelDistance; }
};
