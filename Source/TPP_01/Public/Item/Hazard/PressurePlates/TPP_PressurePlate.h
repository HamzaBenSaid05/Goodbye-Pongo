#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Hazards/IPowerSwitch.h"
#include "TPP_PressurePlate.generated.h"

UENUM(BlueprintType)
enum class EPressurePlateType : uint8
{
	S, L
};

UCLASS()
class TPP_01_API ATPP_PressurePlate : public AActor, public IIPowerSwitch
{
	GENERATED_BODY()

public:
	ATPP_PressurePlate();
	virtual void SetHazardManager(class ATPP_ElectricityManager* ElectricityManager) override;
	UStaticMeshComponent* GetPlateMesh() const { return PlateMesh; }
	class UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayAnimation();
	//====AudioTag====
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	FGameplayTag TagPress;
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	FGameplayTag TagUnPress;
	
	UPROPERTY(BlueprintReadWrite, Category = "Pressure Plate")
	bool bIsPlatePressed;

	bool bIsActivated;
protected:
	UPROPERTY()
	class ATPP_ElectricityManager* ElectricityManagerRef;

	virtual void BeginPlay() override;

	bool GetIsPressureEnough(TArray<AActor*>& Actors);

	UPROPERTY(EditAnywhere)
	EPressurePlateType PressurePlateType;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* CollisionBox;

	UPROPERTY()
	UAudioComponent* AudioComp;
	//====End AudioTag====

	//====SoundComponent====
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UTPP_SoundComponent> SoundComponent;
	//====End SoundComponent====
	
	UPROPERTY(VisibleInstanceOnly, Category = "Pressure Plate")
	TArray<AActor*> OverlappingActors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PlateMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SurfacePlateMesh;

	UFUNCTION()
	virtual void SwitchElectricity_Implementation() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                    bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                          int32 OtherBodyIndex);
};
