#include "Camera/TPP_CameraSpline.h"

#include "Camera/TPP_CameraSplineComponent.h"
#include "Camera/TPP_CameraSplineMetadata.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "EventProxies/TPP_RoomsProxySubsystem.h"
#include "EventProxies/RoomsEvents/TPP_EnteredRoomEvent.h"
#include "EventProxies/RoomsEvents/TPP_ExitRoomEvent.h"
#include "Kismet/GameplayStatics.h"
#include "RoomManagement/TPP_RoomVolume.h"

ATPP_CameraSpline::ATPP_CameraSpline()
{
	SplineComponent = CreateDefaultSubobject<UTPP_CameraSplineComponent>(TEXT("CameraSplineComp"));
	RootComponent = SplineComponent;

	SplineMetadata = CreateDefaultSubobject<UTPP_CameraSplineMetadata>(TEXT("CameraSplineMetadata"));
}

void ATPP_CameraSpline::PostLoad()
{
	Super::PostLoad();
	if (SplineComponent && SplineMetadata)
	{
		// Forces the component to see new metadata if lost
		SplineComponent->UpdateSpline();
	}
}

void ATPP_CameraSpline::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();
	if (SplineComponent && SplineMetadata) { SplineMetadata->Fixup(SplineComponent->GetNumberOfSplinePoints(), SplineComponent); }
}
#if WITH_EDITOR
void ATPP_CameraSpline::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	// Sync if you change the spline in the editor
	if (SplineComponent && SplineMetadata) { SplineMetadata->Fixup(SplineComponent->GetNumberOfSplinePoints(), SplineComponent); }
}
#endif
void ATPP_CameraSpline::FillArrayWithSplinePosition(TArray<FVector>& InSplinePosition) const
{
	int numPoints = SplineComponent->GetNumberOfSplinePoints();
	for (int i = 0; i < numPoints; i++)
	{
		FVector splinePointLocation = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
		InSplinePosition.Add(splinePointLocation);
	}
}

void ATPP_CameraSpline::GetInterpolatedCameraData(const float& Distance, float& OutBias, FRotator& OutRotation, float& OutSpringArmLength,
                                                  float& OutYOffset,
                                                  float& OutZOffset, float& OutBiasInterpSpeed, float& OutLocationInterpSpeed,
                                                  float& OutRotationInterpSpeed, AActor*& OutActor) const
{
	if (SplineMetadata && SplineComponent)
	{
		// Turn the distance into "InputKey"
		float InputValueKey = SplineComponent->GetInputKeyValueAtDistanceAlongSpline(Distance);
		float TotalLen = SplineComponent->GetSplineLength();
		//UE_LOG(LogCameraSpline, Warning, TEXT("Distance: %f / %f | InputKey: %f"), Distance, TotalLen, InputValueKey);

		// Evaluate the curves at that exact point
		// Return Bias from InputKey
		OutBias = SplineMetadata->CameraBias.Eval(InputValueKey);

		// Return Rotation from InputKey
		float TempP = SplineMetadata->CameraRotationPitch.Eval(InputValueKey);
		float TempY = SplineMetadata->CameraRotationYaw.Eval(InputValueKey);
		float TempR = SplineMetadata->CameraRotationRoll.Eval(InputValueKey);
		float P = TempP != 0.f ? TempP : OutRotation.Pitch;
		float Y = TempY != 0.f ? TempY : OutRotation.Yaw;
		float R = TempR != 0.f ? TempR : OutRotation.Roll;

		OutRotation = FRotator(P, Y, R);

		// Return Spring Arm lenght from InputKey
		float TempSprinArmLenght = SplineMetadata->CameraSpringArmLength.Eval(InputValueKey);
		OutSpringArmLength = TempSprinArmLenght > 0 ? TempSprinArmLenght : OutSpringArmLength;

		// Return Y Offset from InputKey
		float TempYOffset = SplineMetadata->CameraYOffset.Eval(InputValueKey);
		OutYOffset = TempYOffset != 0 ? TempYOffset : OutYOffset;

		// Return Z Offset from InputKey
		float TempZOffset = SplineMetadata->CameraZOffset.Eval(InputValueKey);
		OutZOffset = TempZOffset != 0 ? TempZOffset : OutZOffset;

		// Return Bias Interp Speed from InputKey
		float TempBiasInterpSpeed = SplineMetadata->CameraBiasInterpSpeed.Eval(InputValueKey);
		OutBiasInterpSpeed = TempBiasInterpSpeed > 0 ? TempBiasInterpSpeed : OutBiasInterpSpeed;

		// Return Location Interp Speed from InputKey
		float TempLocationInterpSpeed = SplineMetadata->CameraLocationInterpSpeed.Eval(InputValueKey);
		OutLocationInterpSpeed = TempLocationInterpSpeed > 0 ? TempLocationInterpSpeed : OutLocationInterpSpeed;

		// Return Rotation Interp Speed from InputKey
		float TempRotationInterpSpeed = SplineMetadata->CameraRotationInterpSpeed.Eval(InputValueKey);
		OutRotationInterpSpeed = TempRotationInterpSpeed > 0.f ? TempRotationInterpSpeed : OutRotationInterpSpeed;

		// Return Bias Actor from InputKey
		int32 ActorIndex = FMath::Clamp(InputValueKey, 0, SplineMetadata->CameraBiasActor.Num() - 1);
		if (SplineMetadata->CameraBiasActor.IsValidIndex(ActorIndex)) { OutActor = SplineMetadata->CameraBiasActor[ActorIndex].Get(); }
	}
}

void ATPP_CameraSpline::BeginPlay()
{
	Super::BeginPlay();

	if (UTPP_RoomsProxySubsystem* RoomsProxySubsystem = GetWorld()->GetSubsystem<UTPP_RoomsProxySubsystem>())
	{
		RoomsProxySubsystem->OnRoomEvent.AddDynamic(this, &ATPP_CameraSpline::PlayerEnteredExitRoom);
	}
}

void ATPP_CameraSpline::PlayerEnteredExitRoom(const UTPP_RoomEventBase* RoomEvent)
{
	if (const UTPP_EnteredRoomEvent* EnteredEvent = Cast<UTPP_EnteredRoomEvent>(RoomEvent))
	{
	    if (EnteredEvent->RoomActor && EnteredEvent->RoomActor->GetLevel() == GetLevel())
	    {
	        OnPlayerEnteredRoom.Broadcast();
	    }
	}
	else if (const UTPP_ExitRoomEvent* ExitEvent = Cast<UTPP_ExitRoomEvent>(RoomEvent))
	{
	    if (ExitEvent->RoomActor && ExitEvent->RoomActor->GetLevel() == GetLevel())
	    {
	        OnPlayerExitRoom.Broadcast();
	    }
	}
}

TArray<FVector> ATPP_CameraSpline::PassRoomEdgePointsToCamera() const
{
	TArray<FVector> TempEdgePoints;
	TArray<AActor*> RoomVolumeActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATPP_RoomVolume::StaticClass(), RoomVolumeActors);

	RoomVolumeActors.RemoveAll([this](const AActor* RoomVolumeActor) { return RoomVolumeActor->GetLevel() != GetLevel(); });

	if (RoomVolumeActors.IsEmpty()) return TempEdgePoints;

	if (const ATPP_RoomVolume* RoomVolume = Cast<ATPP_RoomVolume>(RoomVolumeActors[0]))
	{
		for (FVector Point : RoomVolume->GetEdgePoints())
		{
			const FVector TransformedPoint = GetActorTransform().TransformPosition(Point);
			TempEdgePoints.Add(TransformedPoint);
		}
	}

	return TempEdgePoints;
}
