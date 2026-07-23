#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "TPP_SplineDecalActor.generated.h"

USTRUCT(BlueprintType)
struct FSplineDecalData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Height = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Material = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LocationOffset = FVector::ZeroVector;

	static void CheckData(const FSplineDecalData& Data)
	{
#if WITH_EDITOR

		if (!GEngine)
		{
			return;
		}

		if (Data.Height <= 0.f)
		{
			GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Red,
			TEXT("[FSplineDecalData] Height must be greater than 0.")
			);
		}

		if (Data.Width <= 0.f)
		{
			GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Red,
			TEXT("[FSplineDecalData] Width must be greater than 0.")
			);
		}

		if (!Data.Material)
		{
			GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Yellow,
			TEXT("[FSplineDecalData] Material not assigned.")
			);
		}

#endif
	}
};

UCLASS()
class TPP_01_API ATPP_SplineDecalActor : public AActor
{
	GENERATED_BODY()

public:
	ATPP_SplineDecalActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USplineComponent* Spline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decals", meta = (DisplayPriority = "1"))
	TArray<FSplineDecalData> SplineDecalData;

	UFUNCTION(CallInEditor, Category = "Decals", meta = (DisplayPriority = "1"))
	void GenerateDecals();

	UFUNCTION(CallInEditor, Category = "Decals", meta = (DisplayPriority = "1"))
	void ClearDecals() const;

	virtual void BeginPlay() override;
	
private:
	void SetUpDecalComponent(const FTransform& RelativeTransform, const FSplineDecalData& Data);
};