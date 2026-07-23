#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "TPP_Drum.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDrumAcquired, AActor*, Char);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGoThereCancelledFinished, bool, bShouldDeactivateDrum);

UCLASS()
class TPP_01_API ATPP_Drum : public AActor
{
	GENERATED_BODY()

public:
	ATPP_Drum();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
	class UNiagaraComponent* TrailVFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
	class UNiagaraComponent* PlayingVFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
	class UNiagaraComponent* CommandVFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
	class UNiagaraComponent* PylonsVFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoThere")
	float MaxDistance = 1000;

	//====Drum Action====
	UFUNCTION(BlueprintCallable, Category = "DrumAction")
	void ComeHere(const FVector& Position);
	void ComeHereCompleted();

	UFUNCTION(BlueprintCallable, Category = "DrumAction")
	void Stop();

	UFUNCTION(BlueprintNativeEvent, Category="DrumAction")
	void GoThereStarted();

	UFUNCTION(BlueprintNativeEvent, Category = "DrumAction")
	void GoThere(const FVector& MarkerDirection, const FVector& StartPositionMarker, const float& DrumHeight);

	UFUNCTION(BlueprintNativeEvent, Category = "DrumAction")
	void GoThereCompleted();

	UFUNCTION(BlueprintNativeEvent, Category = "DrumAction")
	void GoThereCancelled();
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FGoThereCancelledFinished OnCancelledFinished;

	UFUNCTION(BlueprintCallable, Category = "DrumAction")
	void Interact();

	//====Marker====
	UFUNCTION(BlueprintCallable, Category = "Marker")
	void SetMarker(class ATPP_GoThereMarker* NewMarker) { Marker = NewMarker; }

	UFUNCTION(BlueprintCallable, Category = "Marker")
	class ATPP_GoThereMarker* GetMarker() const { return Marker; }

	//====Valid Position====
	UFUNCTION()
	bool IsPointOnNavMesh(const FVector& Location, const FVector& Extends);

	bool bIsValidPosition = false;
	FVector LastValidPositionComeHere;
	UPROPERTY(BlueprintReadOnly)
	FVector LastValidPositionGoThere;

	//====Check Action====
	UFUNCTION(BlueprintCallable, Category = "DrumAction")
	void EnableGoThere(const bool bShouldActivate);

	UFUNCTION(BlueprintCallable, Category = "DrumAction")
	void EnableInteract(const bool bShouldActivate);
	UFUNCTION()
	void ActiveGoThereStartedSound();
	UFUNCTION()
	void ActiveGoThereSound();
	UFUNCTION(BluePrintCallable)
	void DisableGoThereSound();

	UFUNCTION()
	void DeactivateMarker();

	UFUNCTION(BlueprintCallable)
	void ToggleTrail(bool bShouldActivate);

	UFUNCTION(BlueprintCallable)
	void PlayDrumWave();

	UFUNCTION(blueprintCallable)
	void TogglePylonRotation(bool bShouldActivate);

	UFUNCTION()
	void PlayAcquisitionAnimation();

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class ATPP_GoThereMarker> Marker;

	uint8 bGoThereActive  : 1 = false;
	uint8 bInteractActive : 1 = false;
	UPROPERTY(BlueprintReadWrite)
	uint8 bGoThereInUse   : 1 = false;

	UPROPERTY(EditDefaultsOnly)
	float GoThereDistanceNavMesh = 10.0f;
	
	UFUNCTION()
	void HandleDrumAcquired();
	
	UPROPERTY(BlueprintAssignable)
	FDrumAcquired OnDrumAcquired;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TArray<class UMaterialInstanceDynamic*> MaterialInstances;
	
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Audio",meta=(ClampMin="0", ClampMax="5", UIMin="0", UIMax="5"))
	int32 JaneTrustCall = 5;

private:
	//====Audio====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> DrumPylons;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> DrumSphere;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> DrumTop;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> GoThereConeMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNiagaraComponent> GoThereVFX;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpotLightComponent> GoThereLight;
	
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* ConeMeshMaterialInst;

	UPROPERTY(EditDefaultsOnly, Category="VFX")
	float ActivePylonRotationRate;

	UPROPERTY(EditDefaultsOnly, Category="Drum|Animation")
	TObjectPtr<UAnimMontage> PylonAcquisitionMontage;

	UPROPERTY(EditDefaultsOnly, Category="Drum|Animation")
	TObjectPtr<UAnimMontage> SphereAcquisitionMontage;

	UPROPERTY(EditDefaultsOnly, Category="Drum|Animation")
	TObjectPtr<UAnimMontage> TopAcquisitionMontage;
	
	//====AudioTag====
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	FGameplayTag TagAction;
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	FGameplayTag TagState;
	//====End AudioTag====

	//====AudioComponent====
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class UAudioComponent* AudioCompComeHere;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class UAudioComponent* AudioCompStop;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class UAudioComponent* AudioCompGoThereCompleted;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class UAudioComponent* AudioCompInteract;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class UAudioComponent* AudioCompGoThere;
	//====EndAudioComponent====
	//====SoundComponent====
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UTPP_SoundComponent> SoundComponent;
	//====End SoundComponent====
};
