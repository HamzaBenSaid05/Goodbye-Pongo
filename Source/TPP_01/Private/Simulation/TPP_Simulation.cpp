#include "Simulation/TPP_Simulation.h"

ATPP_Simulation::ATPP_Simulation()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // We are starting it with the proper function
}

void ATPP_Simulation::BeginPlay()
{
	Super::BeginPlay();
	if (bRegistering)
	{
		StartRegistration();
	}
	else if (bAutoStart)
	{
		StartSimulation();
	}
}

void ATPP_Simulation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// === Handling registration ===
	if (bRegistering)
	{
		// Registering keyframe
		KeyframesTimer -= DeltaTime;
		if (KeyframesTimer <= 0.f)
		{
			// Using proper serialization - must match FEx010_Simulation_Keyframe
			FVector CurrentLocation = GetActorLocation();
			FString PositionString = FString::Printf(TEXT("X=%f,Y=%f,Z=%f"),CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z);
			FRotator CurrentRotation = GetActorRotation();
			FString RotationString = FString::Printf(TEXT("Pitch=%f,Yaw=%f,Roll=%f"), CurrentRotation.Pitch, CurrentRotation.Yaw, CurrentRotation.Roll);
			RegistrationOutputString += FString::Printf(TEXT(",\n(Time=%f,Position=(%s), Rotation=(%s))"), Timer, *PositionString, *RotationString);
			
			// Resetting the timer to output the keyframe data
			KeyframesTimer += RegistrationDeltaTime;
		}

		// Overall timer handling
		Timer += DeltaTime;
		if (Timer >= RegistrationDuration)
		{
			// Allow to end up only this tick execution
			Timer = RegistrationDuration;

			RegistrationOutputString += FString::Printf(TEXT(")"));
			auto OutputFilename = FPaths::Combine(FPaths::ProjectDir(), RegistrationFilename);
			FFileHelper::SaveStringToFile(RegistrationOutputString, *OutputFilename);
			SetActorTickEnabled(false);
		}
		
		return;
	}

	// === Handling simulation ===
	// Universal timer handling
	Timer += DeltaTime;
	if (Timer >= SimulationDuration)
	{
		// Allow to end up only this tick execution
		Timer = SimulationDuration;
		SetActorTickEnabled(false);
	}

	// Looping all keyframes pairs	
	for (int32 i = 0; i < Keyframes.Num() - 1; ++i)
	{
		// Checking if the current timer is between the current and the next keyframe
		if (Keyframes[i].Time <= Timer && Keyframes[i + 1].Time >= Timer)
		{
			// The interpolation parameter must be
			// - 0 when the timer is exactly at the current keyframe time stamp (i)
			// - 1 when the timer is exactly at the next frame time stamp (i + 1)
			float InterpolationParam = (Timer - Keyframes[i].Time) / (Keyframes[i + 1].Time - Keyframes[i].Time);
			FVector InterpolatedPosition = FMath::Lerp(Keyframes[i].Position, Keyframes[i + 1].Position, InterpolationParam);
			FQuat InterpolatedRotation = FQuat::Slerp(Keyframes[i].Rotation.Quaternion(), Keyframes[i + 1].Rotation.Quaternion(), InterpolationParam);

			// Updating position and rotation
			SetActorLocation(InterpolatedPosition);
			SetActorRotation(InterpolatedRotation);

			// No need to continue the loop as we found the right keyframes
			break;
		}

		// Handling cases where duration is higher than registration time using the last keyframe data
		if (i == Keyframes.Num() - 2 && Keyframes[i + 1].Time < Timer)
		{
			// Updating position and rotation to the last registered location/rotation
			SetActorLocation(Keyframes[i + 1].Position);
			SetActorRotation(Keyframes[i + 1].Rotation);

			// No need to continue simulation			
			SetActorTickEnabled(false);
		}		
	}	
}

void ATPP_Simulation::StartSimulation()
{
	Timer = 0.f;
	if (IsSimulationValid())
	{		
		SetActorTickEnabled(true);
	}
	else
	{
		// This error shows up if the times of the keyframes are not in an incrementing order
		UE_LOG(LogTemp, Error, TEXT("Invalid simulation, please check the keyframes times"));
	}
}

void ATPP_Simulation::StartRegistration()
{
	Timer = 0.f;
	RegistrationOutputString = FString::Printf(TEXT("((Time=%f,Position=(%s), Rotation=(%s))"), Timer, *GetActorLocation().ToString(), *GetActorRotation().ToString());
	KeyframesTimer = RegistrationDeltaTime;
	SetActorTickEnabled(true);	
}

bool ATPP_Simulation::IsSimulationValid()
{
	for (int32 i = 0; i < Keyframes.Num() - 1; ++i)
	{
		// Adjacent keyframes can't have the same or higher time stamps
		if (Keyframes[i].Time >= Keyframes[i + 1].Time)
		{
			return false;
		}
	}
	return true;
}

