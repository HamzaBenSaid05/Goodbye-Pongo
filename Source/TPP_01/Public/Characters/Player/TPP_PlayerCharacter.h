#pragma once

#include "CoreMinimal.h"
#include "Characters/TPP_CharacterBase.h"
#include "Components/TimelineComponent.h"
#include "Drum/TPP_Drum.h"
#include "TPP_PlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractedWithMockDrum);

UCLASS()
class TPP_01_API ATPP_PlayerCharacter : public ATPP_CharacterBase
{
	GENERATED_BODY()

public:
	ATPP_PlayerCharacter();

	FORCEINLINE const TObjectPtr<class ATPP_Drum>& GetDrum() const { return Drum; }
	
	virtual void Interact(TObjectPtr<class UTPP_InteractAction_DA> Data) override;
	UPROPERTY(BlueprintAssignable)
	FOnInteractedWithMockDrum OnInteractedWithMockDrum;

	UFUNCTION(BlueprintCallable, Category = "Drum")
	void SetActiveDrum(bool Active);
	UFUNCTION(BlueprintCallable, Category = "Drum")
	bool GetIsActiveDrum() const { return bDrumActive; }
	bool GetIsLanded() const { return bJustLanded; }
	//====DrumAction====
	UFUNCTION(BlueprintCallable, Category = "DrumAction")
	void ComeHereDrumAction();

	UFUNCTION(BlueprintCallable, Category = "DrumAction")
	void ComeHereDrumCompleted();

	UFUNCTION(BlueprintCallable, Category = "DrumAction")
	void StopDrumAction();

	UFUNCTION(BlueprintCallable, Category = "DrumAction")
	void PongoInteractAction();

	UFUNCTION(BlueprintCallable, Category = "DrumAction")
	void GoThereAction();

	UFUNCTION(BlueprintCallable, Category = "DrumAction")
	void GoThereActionStarted();

	UFUNCTION(BlueprintCallable, Category = "DrumAction")
	void GoThereActionCompleted();

	UFUNCTION(BlueprintCallable, Category = "DrumAction")
	void GoThereActionCancelled();

	//====DrumAction====
	UFUNCTION(BlueprintCallable, Category = "Pet")
	void PetPongo();

	UFUNCTION(BlueprintCallable, Category = "Drum")
	void SetDrum(ATPP_Drum* DrumReference);

	UFUNCTION()
	void PlayDrumStateSound(bool active);
	UFUNCTION(BlueprintCallable, Category = "Drum")
	void PlayDrumAcquisitionAnimation() const;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void Landed(const FHitResult& Hit) override;
	bool CanJump();

	UPROPERTY(BlueprintReadOnly)
	FVector SavedScaleOrbitObject;
	
protected:
	//UI
	void ActivateWidget(FVector PongoLocation);

	void DeactivateWidget();

	UPROPERTY(VisibleAnywhere, Category="UI")
	TObjectPtr<class UWidgetComponent> WidgetComp;
	UPROPERTY(EditAnywhere, Category="UI")
	float ZOffetWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Drum")
	TObjectPtr<class ATPP_Drum> Drum;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* DrumPivot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* DrumOrbitObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meshes")
	TArray<TObjectPtr<UStaticMeshComponent>> DrumAsset;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Crouch")
	float CrouchRadius = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Crouch")
	float CrouchHalfHeight = 30.0f;

	float SavedStartRadius;
	float SavedStartHalfHeight;
	float SavedDistancePlayerDrum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	float InteractDragRadius;

	//UI
	virtual void VisualizeUIItemInteraction() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragBackCheck")
	float ZOffestDragCheckBox = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragBackCheck")
	float YSizeDragCheckBox = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragBackCheck")
	float XSizeDragCheckBox = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragBackCheck")
	float ZSizeDragCheckBox = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragBackCheck")
	float ForwardOffetDragCheckBox = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragBackCheck")
	float DragStuckTimeThreshold = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragBackCheck")
	float StuckDistanceTolerance = 2.0f;
	void CheckDragBlock() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "VFX|Drum|Bracelet")
	TObjectPtr<class UTPP_DrumCommandsColors> DrumCommandsColorsDA;
	UPROPERTY(EditDefaultsOnly, Category = "VFX|Drum|Bracelet")
	TObjectPtr<UCurveFloat> BraceletFadeOutCurve;
	UPROPERTY(EditDefaultsOnly, Category = "VFX|Drum|Bracelet")
	int32 BraceletMaterialSlotIndex = INDEX_NONE;
	UPROPERTY()
	UMaterialInstanceDynamic* BraceletMaterialInstance;
	UPROPERTY()
	UMaterialInstanceDynamic* DrumMaterialInstance;
	
	UPROPERTY()
	UTimelineComponent* BraceletFadeoutTimeline;
	
	UFUNCTION()
	void ChangeBraceletColor(const enum ETPP_DrumCommands Command);
	UFUNCTION()
	void HandleBraceletFadeOutProgress(float Output);
	UFUNCTION()
	void OnBraceletFadeOutFinished();
	
	FLinearColor StartingBraceletColor;
	FLinearColor CurrentBraceletColor;

	FVector LastUIUpdateLocation = FVector::ZeroVector;
	//====DrumCooldown====
	FTimerHandle ComeHereTimerHandle;
	FTimerHandle StopTimerHandle;
	FTimerHandle GoThereTimerHandle;
	FTimerHandle GoThereSoundStartTimerHandle;
	FTimerHandle PongoInteractTimerHandle;
	

	uint8 bDrumActive          : 1 = false;
	uint8 bDrum                : 1 = false;
	uint8 bCanUseComeHere      : 1 = true;
	uint8 bCanUseStop          : 1 = true;
	uint8 bCanUseGoThere       : 1 = true;
	uint8 bCanUsePongoInteract : 1 = true;

	UFUNCTION()
	void SetCanUseComeHere() { bCanUseComeHere = true; }

	UFUNCTION()
	void SetCanUseStop() { bCanUseStop = true; }

	UFUNCTION()
	void SetCanUseGoThere() { bCanUseGoThere = true; }

	UFUNCTION()
	void SetCanUsePongoInteract() { bCanUsePongoInteract = true; }

	UFUNCTION()
	void ActivaGoThereUpdate() const {Drum->ActiveGoThereSound();}

	UPROPERTY(EditDefaultsOnly)
	float ComeHereCooldown = 1.5f;
	UPROPERTY(EditDefaultsOnly)
	float StopCooldown = 1.5f;
	UPROPERTY(EditDefaultsOnly)
	float GoThereCooldown = 1.5f;
	UPROPERTY(EditDefaultsOnly)
	float PongoInteractCooldown = 1.5f;
	//====DrumCooldown====

	// -- Pet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Petting", meta = (AllowPrivateAccess = "true"))
	float PetInteractionLenght = 150.f;
	bool bIsPetting = false;

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY()
	TObjectPtr<class ATPP_PlayerController> PlayerController;
	UPROPERTY(EditDefaultsOnly, Category = "PettingAction")
	TObjectPtr<UAnimMontage> PetMontage;
	UPROPERTY(EditAnywhere, Category = "Petting")
	float PetPercentValue = 5.f;
	UFUNCTION()
	void OnPetFinished();
	
	//====DrumAnimation====
	UFUNCTION()
	void OnOrbitStart();

	UFUNCTION()
	void OnOrbitEnd();

	FTimeline OrbitTimeline;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveFloat> OrbitCurve;

	UPROPERTY(EditDefaultsOnly, Category="Drum|Animation")
	TObjectPtr<UAnimMontage> BodyAcquisitionMontage;

	UFUNCTION()
	void HandleOrbitProgress(float Value);
	//====DrumAnimation====
	
	//====AudioTag====
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	FGameplayTag TagBongoState;
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	FGameplayTag TagPetPongo;
	UPROPERTY(EditAnywhere, Category = "Audio")
	float DurationFadeOutDrum = 0.35;
	UPROPERTY(EditAnywhere, Category = "Audio")
	float VolumeFadeOutDrum = 0.5;
	//====End AudioTag

	//====Audio Componenet====
	UPROPERTY()
	UAudioComponent* AudioComPressed;
	UPROPERTY()
	UAudioComponent* AudioComUnPressed;
	//====End Audio Componenet====
	
	//====Interact====
	virtual void ItemMediumInteract() override;
	virtual void ItemMediumEndInteraction() override { EndDragAction(); }

	virtual void ItemLargeInteract() override
	{
	};

	virtual void ItemLargeEndInteraction() override
	{
	};

	virtual void CuriosityObjectInteract() override { if (GetInteractableItem() == nullptr) { PickUpAction(); } }
	virtual void CuriosityObjectEndInteraction() override { DropAction(); }
	//====End Interact====

	virtual void CheckBackwardBlock() override;

	//==== Drag  ====
	UPROPERTY(EditDefaultsOnly, Category = "Drag")
	float DragApproachAcceptanceRadius = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = "Drag")
	float DragPreApproachAcceptanceRadius = 60.f;

	FTimerHandle DragApproachTimerHandle;
	bool bIsApproachingDragTarget = false;
	FVector DragApproachTargetLocation = FVector::ZeroVector;
	FRotator DragApproachTargetRotation = FRotator::ZeroRotator;
	FVector DragPreApproachLocation;
	bool bIsInPreApproach = false;
	UFUNCTION()
	void TickDragApproach(float DeltaSeconds);
	//==== Drag ====
	uint8 bJustLanded : 1 = true;
	uint8 bIsCrouching : 1 = false;
	float DragStuckTimer = 0.f;
	FVector LastStuckCheckPosition = FVector::ZeroVector;
	
};
