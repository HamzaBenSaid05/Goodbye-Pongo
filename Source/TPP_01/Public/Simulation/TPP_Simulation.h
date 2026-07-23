#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPP_Simulation.generated.h"

USTRUCT()
struct FSimulation_Keyframe
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Time;
	
	UPROPERTY(EditAnywhere)
	FVector Position;

	UPROPERTY(EditAnywhere)
	FRotator Rotation;	
};

UCLASS()
class TPP_01_API ATPP_Simulation : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FSimulation_Keyframe> Keyframes;

	// Duration of the simulation
	UPROPERTY(EditAnywhere)
	float SimulationDuration = 5.f;

	// When enabled, the actor will register the positions
	UPROPERTY(EditAnywhere)
	bool bRegistering = false;

	// When enabled, the actor will autostart the simulation
	UPROPERTY(EditAnywhere)
	bool bAutoStart = false;

	// Interval of time between registered keyframes 
	UPROPERTY(EditAnywhere, meta=(EditCondition="bRegistering"))
	float RegistrationDeltaTime = 0.1f;
	
	// Interval of time between registered keyframes 
	UPROPERTY(EditAnywhere, meta=(EditCondition="bRegistering"))
	float RegistrationDuration = 3.0f;
	
	// Output file for registration. Path is relative to the project directory
	UPROPERTY(EditAnywhere, meta=(EditCondition="bRegistering"))
	FString RegistrationFilename = TEXT("registration.txt");

public:
	ATPP_Simulation();

	virtual void BeginPlay() override;	
	virtual void Tick(float DeltaTime) override;

	// Starting simulation by BP
	UFUNCTION(BlueprintCallable)
	void StartSimulation();

	UFUNCTION(BlueprintCallable)
	void StartRegistration();

	// Checking simulation validity
	bool IsSimulationValid();

private:
	// Global timer, used to simulate or keep track of the overall registration timing
	float Timer = 0.f;

	// Registration timer used to trigger the output of the data of the single keyframe
	float KeyframesTimer = 0.f;

	// Output used for the registration
	FString RegistrationOutputString;
};
