#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "TPP_CameraSplineComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPP_01_API UTPP_CameraSplineComponent : public USplineComponent
{
	GENERATED_BODY()

public:
	// Override to tell the editor where the data is
	virtual USplineMetadata* GetSplinePointsMetadata() override;
	virtual const USplineMetadata* GetSplinePointsMetadata() const override;

	// Enable standard spline point types
	virtual TArray<ESplinePointType::Type> GetEnabledSplinePointTypes() const override;
};
