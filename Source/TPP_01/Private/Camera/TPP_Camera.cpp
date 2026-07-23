#include "Camera/TPP_Camera.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Camera/TPP_CameraSpline.h"
#include "Components/SplineComponent.h"
#include "Components/TPP_CameraZoomComponent.h"

DEFINE_LOG_CATEGORY(LogPlayerCamera);

ATPP_Camera::ATPP_Camera()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SetRootComponent(SpringArmComp);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(RootComponent);
	CameraZoomComponent = CreateDefaultSubobject<UTPP_CameraZoomComponent>(TEXT("Camera Zoom Component"));
}

void ATPP_Camera::SetPlayerRef(AActor* Character)
{
	if (!Character) return;

	PlayerRef = Character;
	SmoothedObjectLocation = PlayerRef->GetActorLocation();
	SetActorRotation((-FVector::RightVector).ToOrientationRotator());
}

void ATPP_Camera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsInUse) return;

	if (!PlayerRef)
	{
		UE_LOG(LogPlayerCamera, Log, TEXT("%s CharacterRef references are null"), *GetName());
		return;
	}

	if (!SplineComponent)
	{
		UE_LOG(LogPlayerCamera, Log, TEXT("SplineComponent references are null"));
		return;
	}
	if (!CameraSpline)
	{
		UE_LOG(LogPlayerCamera, Log, TEXT("CameraSpline references are null"));
		return;
	}

	TargetPoint = FindCameraBias(DeltaSeconds);
	UpdateCameraPosition(DeltaSeconds);
}

void ATPP_Camera::BeginPlay()
{
	Super::BeginPlay();

	SplineComponent = CameraSpline->SplineComponent;

	CameraSpline->FillArrayWithSplinePosition(SplinePosition);

	CameraSpline->OnPlayerEnteredRoom.AddDynamic(this, &ATPP_Camera::PlayerEnteredThisRoom);
	CameraSpline->OnPlayerExitRoom.AddDynamic(this, &ATPP_Camera::PlayerExitThisRoom);
	EdgePoints = CameraSpline->PassRoomEdgePointsToCamera();

	// This makes it so when the player spawns IN the room volume this camera will be triggered
	if (PlayerRef && CameraSpline->IsOverlappingActor(PlayerRef))
		PlayerEnteredThisRoom();
}

void ATPP_Camera::UpdateCameraPosition(const float& DeltaSeconds)
{
	if (EdgePoints.IsEmpty())
	{
		UE_LOG(LogPlayerCamera, Display, TEXT("EdgePoints array is empty, cannot update camera position"));
		return;
	}
	// Corridor vector from start to end and its normalized direction
	const FVector RoomDistance = EdgePoints[1] - EdgePoints[0];
	const float RoomLength = RoomDistance.Size();
	const FVector RoomDirection = RoomDistance.GetSafeNormal();

	// Distance based on player for camera data info
	const FVector DistancePlayerToStartRoom = PlayerRef->GetActorLocation() - EdgePoints[0];
	const float PlayerProjectionOnRoom = FVector::DotProduct(DistancePlayerToStartRoom, RoomDirection);
	const float PlayerAlpha = FMath::Clamp(PlayerProjectionOnRoom / RoomLength, 0.f, 1.f);
	float PlayerDistanceAlongSpline = PlayerAlpha * SplineComponent->GetSplineLength();

	// Vector from corridor start to the target point
	const FVector TargetPointToRoomVector = TargetPoint - EdgePoints[0];

	// Project the target onto the corridor using a dot product
	const float TargetPointProjectionOnRoom = FVector::DotProduct(TargetPointToRoomVector, RoomDirection);

	// Normalized progression along the corridor 
	const float TargetPointAlpha = FMath::Clamp(TargetPointProjectionOnRoom / RoomLength, 0.f, 1.f);

	// Point on the corridor aligned with the target's progression
	const FVector TargetPointProjected = EdgePoints[0] + TargetPointAlpha * RoomDistance;
	// Point on the corridor aligned with the player's progression
	const FVector PlayerPointProjected = EdgePoints[0] + PlayerAlpha * RoomDistance;

	// Spline distance based on alpha
	float TargetPointSplineDistance = TargetPointAlpha * SplineComponent->GetSplineLength();

	for (int32 i = 0; i < Iterations; i++)
	{
		FVector CurrentTargetPointSplinePoint = SplineComponent->GetLocationAtDistanceAlongSpline(
			 TargetPointSplineDistance,
			 ESplineCoordinateSpace::World
			);
		FVector CurrentPlayerSplinePoint = SplineComponent->GetLocationAtDistanceAlongSpline(
		                                                                                     PlayerDistanceAlongSpline,
		                                                                                     ESplineCoordinateSpace::World
		                                                                                    );
		// How far is this point from the plane
		// Positive value we are ahead of the plane, negative we are behind.
		float TargetPointError = FVector::DotProduct(CurrentTargetPointSplinePoint - TargetPointProjected, RoomDirection);
		float PlayerError = FVector::DotProduct(CurrentPlayerSplinePoint - PlayerPointProjected, RoomDirection);

		// Adjust the distance by subtracting the error
		TargetPointSplineDistance -= TargetPointError;
		PlayerDistanceAlongSpline -= PlayerError;

		// Clamp to ensure we stay valid
		TargetPointSplineDistance = FMath::Clamp(TargetPointSplineDistance, 0.f, SplineComponent->GetSplineLength());
		PlayerDistanceAlongSpline = FMath::Clamp(PlayerDistanceAlongSpline, 0.f, SplineComponent->GetSplineLength());
	}
	// Find the closest point on the camera spline to the projected corridor point
	FVector TargetPointSplineIntersection =
		SplineComponent->GetLocationAtDistanceAlongSpline(
		                                                  TargetPointSplineDistance,
		                                                  ESplineCoordinateSpace::World
		                                                 );
	const FVector PlayerPointSplineIntersection =
		SplineComponent->GetLocationAtDistanceAlongSpline(
		                                                  PlayerDistanceAlongSpline,
		                                                  ESplineCoordinateSpace::World
		                                                 );

	// Corridor direction flattened on the horizontal plane
	FVector HorizontalRoomVector = RoomDistance;
	HorizontalRoomVector.Z = 0.f;
	HorizontalRoomVector.Normalize();

	const FRotationMatrix SplineRotMatrix(FinalSplineRotation);

	const FVector Right = SplineRotMatrix.GetUnitAxis(EAxis::Y);
	const FVector Up = SplineRotMatrix.GetUnitAxis(EAxis::Z);

	FVector CompensatedTargetPointPosition = TargetPointSplineIntersection;

	// Retrieve camera data from the spline at the spline distance
	AActor* TempActor = nullptr;
	float TempBias = 0.f;
	FinalSplineRotation = GetActorRotation();
	CameraSpline->GetInterpolatedCameraData(
	                                        PlayerDistanceAlongSpline,
	                                        TempBias,
	                                        FinalSplineRotation,
	                                        FinalCameraSpringArm,
	                                        FinalCameraYOffset,
	                                        FinalCameraZOffset,
	                                        BiasInterpSpeed,
	                                        LocationInterpSpeed,
	                                        RotationInterpSpeed,
	                                        TempActor
	                                       );
	if (FinalCameraSpringArm > 0.f)
		SpringArmComp->TargetArmLength = FinalCameraSpringArm;
	CompensatedTargetPointPosition += Right * FinalCameraYOffset;
	CompensatedTargetPointPosition += Up * FinalCameraZOffset;

	// Apply spline-driven camera parameters
	if (TempActor)
	{
		CurrentObjectRef = TempActor;
		TargetBiasFromSpline = TempBias;
	}
	else
	{
		// Calculate Distance from Target Point and Player
		FVector PlayerPos = PlayerRef->GetActorLocation();
		FVector PlayerToCamera = CompensatedTargetPointPosition - PlayerPos;
		// Saving the spline height
		float OriginalSplineZ = CompensatedTargetPointPosition.Z;
		float OriginalSplineX = CompensatedTargetPointPosition.X;
		FVector PlayerToCamera2D = PlayerToCamera;
		PlayerToCamera2D.Z = 0.f;
		PlayerToCamera2D.X = 0.f;
		float Distance = PlayerToCamera2D.Size();
		if (Distance >= MaxDistanceFromPlayer)
		{
			// Applying offset to mantain max distance
			FVector NewXY = PlayerPos + (PlayerToCamera2D.GetSafeNormal() * MaxDistanceFromPlayer);

			CompensatedTargetPointPosition.Y = NewXY.Y;
			CompensatedTargetPointPosition.X = OriginalSplineX;
			CompensatedTargetPointPosition.Z = OriginalSplineZ;
		}
		TargetBiasFromSpline = 0.f;
	}

	if (FinalSplineRotation != FRotator::ZeroRotator)
	{
		SetActorRotation(
		                 FMath::RInterpTo(
		                                  GetActorRotation(),
		                                  FinalSplineRotation,
		                                  DeltaSeconds,
		                                  LocationInterpSpeed
		                                 )
		                );
	}
#if WITH_EDITOR
	FVector PlayerPos = PlayerRef->GetActorLocation();
	PlayerPos.Z = CameraComp->GetComponentLocation().Z;
	// --- Debug --- 
	//DrawDebugLine(
	//				GetWorld(),
	//				CameraComp->GetComponentLocation(),
	//				PlayerPos,
	//				FColor::Yellow
	//);
	//DrawDebugSphere(
	//				GetWorld(),
	//				CompensatedTargetPointPosition,
	//				25.f,
	//				16,
	//				FColor::Yellow,
	//				false
	//			   );
	// --- Debug ---
#endif
	// Interpolation of Location to Final Spline Position
	SetActorLocation(
	                 FMath::VInterpTo(
	                                  GetActorLocation(),
	                                  CompensatedTargetPointPosition,
	                                  DeltaSeconds,
	                                  LocationInterpSpeed
	                                 )
	                );
}

FVector ATPP_Camera::FindCameraBias(const float& DeltaSeconds)
{
	FVector PlayerPos = PlayerRef->GetActorLocation();

	CameraBiasPlayerToObject = FMath::FInterpTo(
	                                            CameraBiasPlayerToObject,
	                                            TargetBiasFromSpline,
	                                            DeltaSeconds,
	                                            BiasInterpSpeed
	                                           );

	if (!CurrentObjectRef)
	{
		CurrentAlpha =
			FMath::FInterpTo(
			                 CurrentAlpha,
			                 0.f,
			                 DeltaSeconds,
			                 BiasInterpSpeed
			                );
		SmoothedObjectLocation =
			FMath::VInterpTo(
			                 SmoothedObjectLocation,
			                 PlayerPos,
			                 DeltaSeconds,
			                 BiasInterpSpeed
			                );
		return PlayerPos;
	}

	FVector RawObjectPos = CurrentObjectRef->GetActorLocation();

	// This soft the ref change between Companion and TempActor 
	SmoothedObjectLocation = FMath::VInterpTo(
	                                          SmoothedObjectLocation,
	                                          RawObjectPos,
	                                          DeltaSeconds,
	                                          BiasInterpSpeed
	                                         );

	// Smoothly interpolate the current alpha toward the desired CameraBias
	// This avoids camera snapping when CameraBias changes
	CurrentAlpha = FMath::FInterpTo(
	                                CurrentAlpha,
	                                CameraBiasPlayerToObject,
	                                DeltaSeconds,
	                                BiasInterpSpeed
	                               );

	// Find the "percentage" of the position based on alpha between player and companion 
	return FMath::Lerp(
	                   PlayerPos,
	                   RawObjectPos,
	                   CurrentAlpha
	                  );
}

void ATPP_Camera::PlayerEnteredThisRoom()
{
	UE_LOG(LogTemp, Display, TEXT("%s triggered enter"), *GetName());
	OnCameraChange.Broadcast(this, BlendTime);
	bIsInUse = true;
}

void ATPP_Camera::PlayerExitThisRoom() { bIsInUse = false; }

void ATPP_Camera::GetEdgePointsFromSpline(const TArray<FVector>& InEdgePoints) { EdgePoints = InEdgePoints; }

void ATPP_Camera::CalculateEdgePointsFromSpline()
{
	FVector SplineOrigin;
	FVector SplineBoxExtent;
	
	CameraSpline->GetActorBounds(false, SplineOrigin, SplineBoxExtent);

	EdgePoints.Add(SplineOrigin - SplineBoxExtent);
	EdgePoints.Add(SplineOrigin + SplineBoxExtent);
}
