#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "TPP_SplineLinearDecal.generated.h"

USTRUCT(BlueprintType)
struct FSplineLinearDecalData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Material = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color = FLinearColor::White;

	static void CheckData(const FSplineLinearDecalData& Data)
	{
#if WITH_EDITOR

		if (!GEngine)
		{
			return;
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
class TPP_01_API ATPP_SplineLinearDecal : public AActor
{
	GENERATED_BODY()

public:
	ATPP_SplineLinearDecal();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USplineComponent* Spline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decals", meta = (DisplayPriority = "1"))
	FSplineLinearDecalData SplineLinearDecalData;

	UFUNCTION(CallInEditor, Category = "Decals", meta = (DisplayPriority = "1"))
	void GenerateSingleDecal();

	UFUNCTION(CallInEditor, Category = "Decals", meta = (DisplayPriority = "1"))
	void ClearDecals() const;

	virtual void BeginPlay() override;
};
