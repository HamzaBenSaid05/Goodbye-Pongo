#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Gameplay/GameModes/TPP_GameMode.h"
#include "Item/Hazard/TPP_Hazard.h"
#include "TPP_BaseStatueHazard.generated.h"

UCLASS()
class TPP_01_API ATPP_BaseStatueHazard : public ATPP_Hazard
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ToggleStatueActive(const bool bShouldBeActive);
	
	UPROPERTY(EditAnywhere)
	bool bShouldSoftReset;

protected:
	ATPP_BaseStatueHazard();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light", meta = (AllowPrivateAccess = true))
	class UNiagaraComponent* StaticVFX;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light", meta = (AllowPrivateAccess = true))
	class USpotLightComponent* SpotLight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light", meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* LightCone;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mesh", meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* HazardMesh;

	UFUNCTION()
	void InitTargets();

	virtual void BeginPlay() override;

	UFUNCTION()
	void KillTargets(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                 int32 OtherBodyIndex,
	                 bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void InitializeStatueAnim();

	UFUNCTION()
	void DeinitializeStatueAnim();

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleLightCone(bool bShouldExpand);

	UPROPERTY(BlueprintReadOnly, Transient)
	FVector InitialLightConeScale;

	UPROPERTY(EditAnywhere, BluePrintReadWrite)
	USceneComponent* Emitter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "DeathSettings")
	EDeathCause DeathCause = EDeathCause::Statue;
	//====Audio====
	UPROPERTY(EditAnywhere, Category="Tag Audio")
	FGameplayTag TagStatue;
private:
	UPROPERTY(EditDefaultsOnly)
	float AngleThreshold;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TObjectPtr<UAnimMontage> ActivationAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	float DeinitPosition;

	UPROPERTY(Transient)
	TArray<AActor*> Targets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hazard", meta = (DisplayPriority = "1"), meta=(AllowPrivateAccess=true))
	float TimeToTriggerReload = 0.5f;

	UPROPERTY(Transient)
	class UMaterialInstanceDynamic* LightConeMI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess=true))
	FLinearColor IdleColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess=true))
	FLinearColor SpottedColor;
	//====Audio====
	UPROPERTY()
	UAudioComponent* AudioCompKill;
	//====Audio====
};
