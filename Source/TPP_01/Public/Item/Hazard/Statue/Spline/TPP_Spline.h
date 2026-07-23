#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPP_Spline.generated.h"

USTRUCT(BlueprintType)
struct FSplinePointParameters
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Movement", meta = (DisplayPriority = "1"))
	float MoveSpeed = 200.f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Movement", meta = (DisplayPriority = "1"))
	int32 Direction = 1;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Movement", meta = (DisplayPriority = "1"))
	FRotator Rotation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Movement", meta = (DisplayPriority = "1"))
	float RotationSpeed;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Movement", meta = (DisplayPriority = "1"))
	bool bHasWaitTime;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Movement", meta = (DisplayPriority = "1"))
	float WaitTime;
};

UCLASS()
class TPP_01_API ATPP_Spline : public AActor
{
	GENERATED_BODY()
	
public:	
	ATPP_Spline();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spline")
	class USplineComponent* SplineComponent;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Movement", meta = (DisplayPriority = "1"))
	TArray<FSplinePointParameters> SplinePointParameters;
};
