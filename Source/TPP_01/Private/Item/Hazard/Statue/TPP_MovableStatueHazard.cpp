#include "Item/Hazard/Statue/TPP_MovableStatueHazard.h"

#include "Components/AudioComponent.h"
#include "Item/Hazard/Statue/Spline/TPP_Spline.h"
#include "Components/SplineComponent.h"
#include "Components/TPP_SoundComponent.h"

ATPP_MovableStatueHazard::ATPP_MovableStatueHazard()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATPP_MovableStatueHazard::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bIsFirstSwitch)
	{
		MoveStatueHazard(DeltaSeconds);
	}
	
}

void ATPP_MovableStatueHazard::BeginPlay()
{
	Super::BeginPlay();

	if (!SplineComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spline non assegnata a MovableStatueHazard!"));
		return;
	}

	if (SplineComp->SplineComponent)
		SplineLength = SplineComp->SplineComponent->GetSplineLength();

	if (SplineComp->SplineComponent)
	{
		FVector StartPos = SplineComp->SplineComponent->GetLocationAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
		SetActorLocation(StartPos);
	}
}

void ATPP_MovableStatueHazard::SwitchPower_Implementation()
{
	if (!bIsFirstSwitch)
	{
		UpdateSplinePointParameters(0);
		InitializeStatueAnim();
		bIsFirstSwitch = true;
	}

	//GetWorld()->GetTimerManager().SetTimer(
	//	ActivityTimer,
	//	this,
	//	&ThisClass::MoveStatueHazard,
	//	GetWorld()->GetDeltaSeconds(),
	//	true
		//);
	else
	{
		//GetWorld()->GetTimerManager().ClearTimer(ActivityTimer);
		bIsFirstSwitch = false;
		DeinitializeStatueAnim();
	}
}

void ATPP_MovableStatueHazard::UpdateSplinePointParameters(int IndexPoint)
{
	if (!SplineComp->SplineComponent) return;

	int32 NumPoints = SplineComp->SplineComponent->GetNumberOfSplinePoints();

	if (IndexPoint < NumPoints)
	{
		SplineParam = SplineComp->SplinePointParameters[IndexPoint];

		int32 NextIndex = IndexPoint + 1;

		if (NextIndex < NumPoints)
		{
			NextSplineLenght = SplineComp->SplineComponent->GetDistanceAlongSplineAtSplinePoint(NextIndex);
		}
		else
		{
			NextSplineLenght = SplineLength;
		}
	}
}

void ATPP_MovableStatueHazard::ChangeSpline(ATPP_Spline* NewSpline)
{
	if (!IsValid(NewSpline))
		return;

	//GetWorld()->GetTimerManager().ClearTimer(ActivityTimer);

	SplineComp = NewSpline;

	if (SplineComp->SplineComponent)
	{
		SplineLength = SplineComp->SplineComponent->GetSplineLength();

		DistanceAlongSpline = 0.f;
		CurrentIndex = 0;
		ElapseTime = 0.f;

		FVector StartPos = SplineComp->SplineComponent->GetLocationAtSplinePoint(
			0,
			ESplineCoordinateSpace::World
		);

		SetActorLocation(StartPos);

		DrawDebugSphere(
			GetWorld(),
			StartPos,
			25.f,          
			12,            
			FColor::Red,
			false,         
			5.f            
		);

		UE_LOG(LogTemp, Warning, TEXT("New StartPos: %s"), *StartPos.ToString());
	}
	bIsFirstSwitch = false; 
	INeedPowerSwitch::Execute_SwitchPower(this);
}

void ATPP_MovableStatueHazard::MoveStatueHazard(float DeltaTime)
{
	//ScanForCharacters();
	if (AudioCompMove == nullptr || !AudioCompMove->IsPlaying())
	{
		FTPP_AudioParameter Parameter = FTPP_AudioParameter();
		Parameter.Int = 3;
		FVector Position = Emitter->GetRelativeLocation() + GetActorLocation();
		SoundComponent->PlaySound(TagStatue,Position,AudioCompMove,Parameter);
	}
	if (SplineParam.bHasWaitTime)
	{
		if (SplineParam.WaitTime > ElapseTime)
		{
			ElapseTime += DeltaTime;
		}
		else
		{
			ElapseTime = 0;
			SplineParam.bHasWaitTime = false;
		}
	}
	else
	{
		DistanceAlongSpline += SplineParam.Direction * SplineParam.MoveSpeed * DeltaTime;
		DistanceAlongSpline = FMath::Clamp(DistanceAlongSpline, 0.f, SplineLength);

		FVector NewPos = SplineComp->SplineComponent->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		FRotator CurrentRot = GetActorRotation();
		FRotator TargetRot = SplineParam.Rotation;

		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, SplineParam.RotationSpeed);
		SetActorLocationAndRotation(NewPos, NewRot, true);
		if (DistanceAlongSpline >= NextSplineLenght)
		{
			CurrentIndex++;

			if (CurrentIndex < SplineComp->SplineComponent->GetNumberOfSplinePoints() - 1)
			{
				UpdateSplinePointParameters(CurrentIndex);
			}
			else
			{
				OnStatueFinishMoving();
			}
		}
	}
}

