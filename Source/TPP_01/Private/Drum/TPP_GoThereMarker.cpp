#include "Drum/TPP_GoThereMarker.h"

#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

class UTPP_AudioEvent_GoThere;
class UNavigationSystemV1;

ATPP_GoThereMarker::ATPP_GoThereMarker()
{
	PrimaryActorTick.bCanEverTick = true;
	MarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MarkerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(MarkerMesh);

	PlayingVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Playing VFX"));
	PlayingVFX->SetupAttachment(GetRootComponent());

	PylonVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PylonVFX"));
	PylonVFX->SetupAttachment(GetRootComponent());
}

void ATPP_GoThereMarker::BeginPlay()
{
	Super::BeginPlay();
	FVector Min, Max;
	MarkerMesh->GetLocalBounds(Min, Max);
	MarkerBounds = Max;
	DisableMarker();
	ForwardOffset += MarkerBounds.Y;
	if (MarkerMaterialBase)
	{
		MarkerMaterial = UMaterialInstanceDynamic::Create(MarkerMaterialBase, this);
		MarkerMesh->SetMaterial(0, MarkerMaterial);
	}
}

void ATPP_GoThereMarker::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Move(DeltaSeconds);
}

FVector ATPP_GoThereMarker::ActiveMarker(const FVector& MarkerDirection, const FVector& StartPositionMarker, const float& MaxDistanceMarker,
                                         const float& StartHeight)
{
	StartPosition = StartPositionMarker;
	CharacterDirection = MarkerDirection;
	if (bIsActive)
		return GetActorLocation();

	DrumHeight = StartHeight;
	MaxDistance = MaxDistanceMarker;
	bIsActive = true;
	InitialDirection = MarkerDirection;
	FVector InitPos = StartPositionMarker;
	FVector EndMarkerPosition = StartPosition - FVector(0, 0, MaxStepDownHeight);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	FHitResult StartHit = LineTraceByChannel(StartPositionMarker, EndMarkerPosition,Params,ECC_Visibility);
	if (StartHit.bBlockingHit)
	{
		InitPos.Z =
			StartHit.ImpactPoint.Z +
			MarkerMesh->Bounds.BoxExtent.Z;
	}
	SetActorLocation(InitPos);
	this->SetActorTickEnabled(true);
	SetVisibilityMarker(true);

	return GetActorLocation();
}

void ATPP_GoThereMarker::DisableMarker()
{
	bIsActive = false;
	bCanClimb = true;
	SetVisibilityMarker(false);
	SetActorTickEnabled(false);
}

void ATPP_GoThereMarker::Move(const float& DeltaSeconds)
{
	FVector CurrentLocation = GetActorLocation();
	//GEngine->AddOnScreenDebugMessage(99, 1.0f, FColor::White, CurrentLocation.ToString());
	FVector Axis = InitialDirection.GetSafeNormal();

	FVector FromStart = CurrentLocation - StartPosition;
	float SignedDistance = FVector::DotProduct(FromStart, Axis);
	// ====CLAMP SIGNED_DISTANCE=====
	SignedDistance = FMath::Clamp(SignedDistance, 0.f, MaxDistance);
	// ====ALPHE SPEEDCURVE=====
	float Alpha = 0.f;
	if (!bCanClimb && bMoveForward)
	{
		Alpha = FMath::Clamp(
		                     (SignedDistance * SignedDistance) / MaxDistanceInside,
		                     0.f,
		                     1.f);
	}
	else if (MaxDistance > 0.f)
	{
		Alpha = FMath::Clamp(
		                     SignedDistance / MaxDistance,
		                     0.f,
		                     1.f);
	}
	float CurrentSpeed = Speed;
	if (SpeedCurve)
		CurrentSpeed *= SpeedCurve->GetFloatValue(Alpha);

	// ====SET DIRECTION=====
	float DirectionSign = 1.f;
	if (!bMoveForward)
		DirectionSign = -1.f;

	FVector MoveVector =
		Axis * DirectionSign *
		CurrentSpeed * DeltaSeconds;

	SetActorRotation(MoveVector.Rotation());

	FVector NewLocation = CurrentLocation + MoveVector;

	FVector DownStart;
	FVector DownEnd;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	//====FORWARD TRACE=====
	//====FORWARD TRACE DISTANCE====
	FVector ForwardStart = CurrentLocation;
	ForwardStart.Z = (CurrentLocation.Z - MarkerBounds.Z) + MinStep;
	FVector ForwardEnd = ForwardStart + Axis * DirectionSign * ForwardOffset;

	//====FORWARD TRACE LINETRACE====
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Visibility);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	FHitResult ForwardHit = LineTraceByObjectType(ForwardStart, ForwardEnd, Params, ObjectQueryParams);
	//DrawDebugLine(GetWorld(), ForwardStart, ForwardEnd, FColor::Red, false, 0.1f);

	if (ForwardHit.GetActor() != nullptr)
	{
		if (ForwardHit.GetActor()->ActorHasTag(MyTag))
		{
			bMoveForward = false;
			return;
		}
		AActor* HitActor = ForwardHit.GetActor();
		//GEngine->AddOnScreenDebugMessage(77, 15.0f, FColor::Red, HitActor->GetName());
		FVector Origin;
		FVector Extent;
		HitActor->GetActorBounds(true, Origin, Extent);
		float TopZ = Origin.Z + Extent.Z;
		float Height = CurrentLocation.Z;
		if (bMoveForward == true) { Height += MaxStepUpHeight; }
		else { Height += MaxStepDownHeight; }

		// ====GO UP====
		if (TopZ < Height) { NewLocation.Z = TopZ; }
		else { bMoveForward = false; }
	}
	DownStart = NewLocation + ((InitialDirection * DirectionSign) * DownOffset);
	DownEnd = DownStart - FVector(0, 0, MaxStepDownHeight);

	// ====CHECKED TRACE=====
	FVector CheckedStart = CurrentLocation;
	CheckedStart.Z = (CurrentLocation.Z - MarkerBounds.Z) + MinStep;
	FVector CheckedEnd =
		StartPosition +
		Axis * MaxDistance;
	CheckedEnd.Z = (CurrentLocation.Z - MarkerBounds.Z) + MinStep;

	FHitResult CheckedHit = LineTraceByObjectType(CheckedStart, CheckedEnd, Params, ObjectQueryParams);
	if (CheckedHit.GetActor() != nullptr)
	{
		AActor* HitActor = CheckedHit.GetActor();
		FVector Origin;
		FVector Extent;
		HitActor->GetActorBounds(true, Origin, Extent);
		float TopZ = Origin.Z + Extent.Z;
		FVector InitHitActor = Origin + Extent;
		FVector EndHitActor = Origin - Extent;
		FBox Box(InitHitActor, EndHitActor);
		//GEngine->AddOnScreenDebugMessage(77, 15.0f, FColor::Red, FString::Printf(TEXT("TopZ: %f, Height %f"), TopZ, MaxHeight + CurrentLocation.Z));

		// ====GO UP====
		if (TopZ > MaxHeight + CurrentLocation.Z)
		{
			MaxDistanceInside = FVector::DistSquared(CurrentLocation, CheckedHit.ImpactPoint);
			if (CheckedHit.GetActor()->ActorHasTag(MyTag))
			{

			}
			else
			{
				//MaxDistanceInside = FVector::DistSquared(CurrentLocation, CheckedHit.ImpactPoint);
				bCanClimb = false;
			}
		}
		else if (Box.IsInside(CheckedHit.ImpactPoint))
		{
			MaxDistanceInside = FVector::DistSquared(CurrentLocation, CheckedHit.ImpactPoint);
			bCanClimb = false;
		}
	}
	else { bCanClimb = true; }

	// ====DOWN TRACE====
	FHitResult DownHit = LineTraceByChannel(DownStart, DownEnd, Params, ECC_Visibility);
	//DrawDebugLine(GetWorld(), DownStart, DownEnd, FColor::Red, false, 0.1f);

	// ====GO DOWN====
	if (DownHit.GetActor() != nullptr)
	{
		if (!DownHit.GetActor()->ActorHasTag(MyTag))
		{
			if (NewLocation.Z > DownHit.ImpactPoint.Z + DrumHeight) { NewLocation.Z = DownHit.ImpactPoint.Z + DrumHeight; }
			else { NewLocation.Z = DownHit.ImpactPoint.Z + DrumHeight; }
		}
	}

	// ====CHANGE DIRECTION====
	if (SignedDistance >= MaxDistance) { bMoveForward = false; }
	else if (SignedDistance <= 0.f) { bMoveForward = true; }

	// ====CHECK DOT====
	float Dot = FVector::DotProduct(Axis, CharacterDirection);

	if (Dot < AngleInfluence)
	{
		InitialDirection = CharacterDirection.GetSafeNormal();
		FHitResult Hit = LineTraceByObjectType(StartPosition, StartPosition + InitialDirection * SignedDistance, Params,ObjectQueryParams);
		DrawDebugLine(GetWorld(), StartPosition, StartPosition + InitialDirection * SignedDistance, FColor::Red, false, 0.1f);
		if (Hit.GetActor() == nullptr)
			NewLocation = StartPosition + InitialDirection * SignedDistance;
		else { NewLocation = Hit.GetActor()->GetActorLocation() + GetActorForwardVector() * -100; }
	}

	// ==== SET ACTOR LOCATION ====
	FVector End = FVector(NewLocation.X, NewLocation.Y, NewLocation.Z);
	SetActorLocation(End);
}

FHitResult ATPP_GoThereMarker::LineTraceByChannel(const FVector& Start, const FVector& End, const FCollisionQueryParams& QueryParams,
                                                  const ECollisionChannel TraceChannel) const
{
	FHitResult Hit;

	if (GetWorld())
	{
		GetWorld()->LineTraceSingleByChannel(
		                                     Hit,
		                                     Start,
		                                     End,
		                                     TraceChannel,
		                                     QueryParams
		                                    );
	}
	return Hit;
}

FHitResult ATPP_GoThereMarker::LineTraceByObjectType(const FVector& Start, const FVector& End, const FCollisionQueryParams& QueryParams,
                                                     const FCollisionObjectQueryParams& ObjectParams) const
{
	FHitResult Hit;

	if (GetWorld())
	{
		GetWorld()->LineTraceSingleByObjectType(
		                                        Hit,
		                                        Start,
		                                        End,
		                                        ObjectParams,
		                                        QueryParams
		                                       );
	}
	return Hit;
}

void ATPP_GoThereMarker::SetColor(FLinearColor NewColor)
{
	if (!bIsActive) return;

	MarkerMaterial->SetVectorParameterValue("Color", NewColor);

	PylonVFX->SetVariableLinearColor("InColor", NewColor);
}

void ATPP_GoThereMarker::SetVisibilityMarker(bool const Visible) const { MarkerMesh->SetVisibility(Visible); }
