#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "TPP_CameraSplineMetadata.generated.h"

UCLASS()
class TPP_01_API UTPP_CameraSplineMetadata : public USplineMetadata
{
	GENERATED_BODY()

public:
	UTPP_CameraSplineMetadata(const FObjectInitializer& ObjectInitializer);

	// USplineMetadata Interface 
	virtual void InsertPoint(int32 Index, float t, bool bClosedLoop) override;
	virtual void UpdatePoint(int32 Index, float t, bool bClosedLoop) override;
	virtual void AddPoint(float InputKey) override;
	virtual void RemovePoint(int32 Index) override;
	virtual void DuplicatePoint(int32 Index) override;
	virtual void CopyPoint(const USplineMetadata* FromSplineMetadata, int32 FromIndex, int32 ToIndex) override;
	virtual void Reset(int32 NumPoints) override;
	virtual void Fixup(int32 NumPoints, USplineComponent* SplineComp) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Data")
	FInterpCurveFloat CameraBias;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Data")
	FInterpCurveFloat CameraRotationPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Data")
	FInterpCurveFloat CameraRotationYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Data")
	FInterpCurveFloat CameraRotationRoll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Data")
	FInterpCurveFloat CameraSpringArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Data")
	FInterpCurveFloat CameraYOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Data")
	FInterpCurveFloat CameraZOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Data")
	FInterpCurveFloat CameraBiasInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Data")
	FInterpCurveFloat CameraLocationInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Data")
	FInterpCurveFloat CameraRotationInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Data")
	TArray<TObjectPtr<AActor>> CameraBiasActor;
};
