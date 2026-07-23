#pragma once

#include "CoreMinimal.h"
#include "Components/TPP_SoundComponent.h"
#include "GameFramework/Actor.h"
#include "TPP_ElectricCircuit.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FPlateCircuit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<class ATPP_CircuitPressurePlate> PressurePlate;
	UPROPERTY(EditAnywhere)
	TArray<class ADecalActor*> Rails;
};

UCLASS()
class TPP_01_API ATPP_ElectricCircuit : public AActor
{
	GENERATED_BODY()

public:
	ATPP_ElectricCircuit();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> EnergySphere;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class ATPP_BasicItem> Door;
	UPROPERTY(EditAnywhere)
	TMap<int, FPlateCircuit> CircuitPressurePlates;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RadiusOffset;
	
	UFUNCTION(BlueprintImplementableEvent)
	void CloseCircuit();

	UPROPERTY(EditAnywhere)
	FGameplayTag TagEletricCircuit;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCurveFloat> SphereMovingCurve;
	UPROPERTY()
	class UTimelineComponent* TimelineComp;
	
	UFUNCTION()
	void StartCircuit();
	
	UFUNCTION()
	void HandleTimelineUpdate(float Output);
	UFUNCTION()
	void HandleTimelineStart();
	UFUNCTION()
	void HandleTimelineEnd();

	UFUNCTION()
	void HandlePressurePlateToggled(class ATPP_CircuitPressurePlate* PressurePlate);
	void GoToNextPlate();

	bool bGoingForward;
	int CurrentRail;
	int CurrentPP;
	int NumPlates;
	UPROPERTY()
	class UMaterialInstanceDynamic* CurrentRailMatInst;

	FVector SphereStartingLocation;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<class ATPP_CircuitPressurePlate*> PressurePlates;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UTPP_SoundComponent> SoundComponent;
};
