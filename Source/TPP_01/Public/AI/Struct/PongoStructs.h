#pragma once
#include "AI/Enum/TPP_PongoEnum.h"
#include "CoreMinimal.h"
#include "PongoStructs.generated.h"

USTRUCT(BlueprintType)
struct FBondingColor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Bonding")
	float BondingValue = 0.f;
	UPROPERTY(EditAnywhere, Category="Bonding")
	FLinearColor BoundingColor;
};

USTRUCT(BlueprintType)
struct FBondingValues
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Bonding Value")
	float PongoJaneMinDistance;
	UPROPERTY(EditAnywhere, Category="Bonding Value")
	float FollowDuration;
	UPROPERTY(EditAnywhere, Category="Bonding Value")
	float MarginOfTolerance;
	UPROPERTY(EditAnywhere, Category="Bonding Value")
	float FirstIdleDuration;
	UPROPERTY(EditAnywhere, Category="Bonding Value")
	float StrollRange;
	UPROPERTY(EditAnywhere, Category="Bonding Value")
	float IdleDurationBeforeDropObject;
	UPROPERTY(EditAnywhere, Category="Bonding Value")
	float MaxIdleDuration;
	UPROPERTY(EditAnywhere, Category="Bonding Value")
	float MinIdleDuration;
};

USTRUCT(BlueprintType)
struct FBondingStep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Bonding")
	float BondingValue = 0.f;
	UPROPERTY(EditAnywhere, Category="Bonding")
	FBondingValues BoundingValues;
};

USTRUCT(BlueprintType)
struct FBondingData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="State")
	ETPP_PongoState State = ETPP_PongoState::Idle;
	UPROPERTY(EditAnywhere, Category="Bonding Range")
	float MinBondingValue = 0.f;
	UPROPERTY(EditAnywhere, Category="Bonding Range")
	float MaxBondingValue = 0.f;
	UPROPERTY(EditAnywhere, Category="Bonding Values")
	TArray<FBondingStep> BondingSteps;
	UPROPERTY(EditDefaultsOnly, Category="Bonding")
	TArray<FBondingColor> BondingColorsData;
	bool operator==(ETPP_PongoState InState) const { return State == InState; }
};
