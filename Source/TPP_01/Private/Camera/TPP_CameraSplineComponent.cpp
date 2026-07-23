#include "Camera/TPP_CameraSplineComponent.h"
#include "Camera/TPP_CameraSpline.h"
#include "Camera/TPP_CameraSplineMetadata.h"

USplineMetadata* UTPP_CameraSplineComponent::GetSplinePointsMetadata()
{
	if (const ATPP_CameraSpline* OwningActor = Cast<ATPP_CameraSpline>(GetOwner())) { return OwningActor->SplineMetadata; }
	return nullptr;
}

const USplineMetadata* UTPP_CameraSplineComponent::GetSplinePointsMetadata() const
{
	if (const ATPP_CameraSpline* OwningActor = Cast<ATPP_CameraSpline>(GetOwner())) { return OwningActor->SplineMetadata; }
	return nullptr;
}

TArray<ESplinePointType::Type> UTPP_CameraSplineComponent::GetEnabledSplinePointTypes() const
{
	// Enable all types of points
	TArray<ESplinePointType::Type> Types;
	Types.Add(ESplinePointType::Linear);
	Types.Add(ESplinePointType::Curve);
	Types.Add(ESplinePointType::Constant);
	Types.Add(ESplinePointType::CurveClamped);
	Types.Add(ESplinePointType::CurveCustomTangent);
	return Types;
}
