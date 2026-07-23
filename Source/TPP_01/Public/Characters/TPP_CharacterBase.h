#pragma once

#include "CoreMinimal.h"
#include "Components/TPP_SoundComponent.h"
#include "GameFramework/Character.h"
#include "Gameplay/GameModes/TPP_GameMode.h"
#include "Interfaces/Killable.h"
#include "Interfaces/Interact/TPP_Interactor.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "TPP_CharacterBase.generated.h"

UCLASS()
class TPP_01_API ATPP_CharacterBase : public ACharacter, public ITPP_Interactor, public IKillable
{
	GENERATED_BODY()

public:
	ATPP_CharacterBase();
	
	UFUNCTION(BlueprintCallable, Category = "SoundComponenet")
	UTPP_SoundComponent* GetSoundComponent() { return SoundComponent; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="PickUp")
	FName SocketName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PickUp")
	TEnumAsByte<ECollisionEnabled::Type> CollisionState = ECollisionEnabled::NoCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	TEnumAsByte<ECollisionChannel> ClimbableChannel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	TEnumAsByte<ECollisionChannel> BoxChannel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	class UPhysicsHandleComponent* PhysicsHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	float DragDistance = 150;
	//---Begin IInteractor Methods
	virtual void Interact(TObjectPtr<class UTPP_InteractAction_DA> Data) override;
	virtual void EndInteract(TObjectPtr<class UTPP_InteractAction_DA> Data) override;
	virtual bool IsInteracting() override;
	//---End IInteractor Methods

	UFUNCTION(BlueprintPure, Category = "Character")
	class ATPP_InteractableItem* GetDetectedItem() const { return DetectedItem; }

	UFUNCTION(BlueprintPure, Category = "Character")
	class ATPP_InteractableItem* GetInteractableItem() const { return InteractableItem; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetIsDragging() const { return bIsDragging; }

	void SetDetectedItem(class ATPP_InteractableItem* ItemToSave) { DetectedItem = ItemToSave; }
	void SetInteractableItem(class ATPP_InteractableItem* ItemToSave) { InteractableItem = ItemToSave; }

	virtual void Kill_Implementation(const bool bShouldSoftReset = false, const ECharacterDeath CharacterDeath = ECharacterDeath::PlayerDeath, const EDeathCause DeathCause = EDeathCause::KillBox) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character")
	float InteractionLength = 100.f;

	UFUNCTION(BlueprintPure, Category = "Character")
	float GetMaxSpeed() const { return SavedVelocity; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	void SetMaxSpeed(float MaxSpeed);

	UFUNCTION(BlueprintPure, Category = "Character")
	float GetMaxAcceleration() const { return SavedAcceleration; }

	void SetMaxAcceleration(float Acceleration) const;

	UFUNCTION(BlueprintCallable, Category = "Character")
	void ResetStartVelocity();
	// TestVariable, Once Testing Completed will be substituted 
	UPROPERTY()
	bool bHasBlockedBackMovement;
	UPROPERTY()
	bool bHasBlockedForMovement;


	UFUNCTION(BlueprintPure)
	class UTPP_DeathComponent* GetDeathComp() const { return DeathComp; }
	UFUNCTION()
	FRotator GetDropRotation();

protected:
	virtual void BeginPlay() override;
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;
	//====Interaction====
	virtual void ItemSmallInteract();
	virtual void ItemSmallEndInteraction();
	virtual void ItemMediumInteract();
	virtual void ItemMediumEndInteraction();
	virtual void ItemLargeInteract();
	virtual void ItemLargeEndInteraction();
	virtual void ItemOrbInteract();
	virtual void ItemOrbEndInteraction();
	virtual void ElectricSwitchInteract();
	virtual void ElectricSwitchEndInteraction();
	virtual void CuriosityObjectInteract();
	virtual void CuriosityObjectEndInteraction();
	//====Interaction====
	//====Trace====
	UFUNCTION()
	virtual FHitResult LineTrace(const FVector& Start, const FVector& End);
	FHitResult BoxTrace(const FVector& Start, const FVector& End, const FVector& Size, const FCollisionQueryParams& Params, ECollisionChannel TraceChannel = ECC_Visibility) const;
	virtual void GetBoxTraceIgnoredActors(TArray<AActor*>& OutIgnored) {}
	// UI
	virtual void VisualizeUIItemInteraction();
	// UI
	virtual void PickUpAction();
	virtual void DropAction();
	virtual void OnItemDetached(ATPP_InteractableItem* Item);

	virtual void ActivateElectricSwitch();
	virtual void PossessedBy(AController* NewController) override;

	//====Tag====
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tags")
	FName NoDropTag = TEXT("NoDrop");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tags")
	FName Attachable = TEXT("Attachable");

	UPROPERTY()
	class ATPP_InteractableItem* DetectedItem;

	UPROPERTY()
	class ATPP_InteractableItem* InteractableItem;

	virtual void Tick(float DeltaSeconds) override;

	virtual void Landed(const FHitResult& Hit) override;
	//====InteractStats====
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interact")
	float ForwardOffset = 100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interact")
	float SizeInteractY = 20.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interact")
	float SizeInteractX = 30.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interact", meta = (ClampMin = "0.1", ClampMax = "1.0", UIMin = "0.1", UIMax = "1.0"))
	float SizeInteractZ = 0.8f;
	//SpeedAreaYet

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	USceneComponent* PickupPoint;

	//====Speed====
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PickUp")
	float PickUpVelocity;
	UPROPERTY(BlueprintReadOnly, Category = "PickUp")
	float SavedVelocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PickUp")
	float PickUpAcceleration;
	UPROPERTY(BlueprintReadOnly, Category = "PickUp")
	float SavedAcceleration;

	FVector ItemBounds;
	bool bHitActor = false;

	UPROPERTY(EditAnywhere, Category="Pickup")
	float HoldBackDistance = 12.f;
	UPROPERTY(EditAnywhere, Category="Pickup")
	float MaxFallDistance = 1000.f;
	virtual void SearchForPedestal();
	virtual void DragAction();
	virtual void CheckBackwardBlock();
	virtual void CheckDragBlock();
	void SetSizeCapsule(float& NewRadius, float& NewHalfHeight);
	UFUNCTION()
	virtual void EndDragAction();
	bool bIsDragging = false;

	FVector ItemScale;
	FVector ItemExtent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PickUp")
	float OffsetPickUp = 30.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PickUp")
	float OffsetDrop = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Drag")
	float DragVelocity = 150;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Drag")
	float ForwardDragOffset = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PickUp")
	float SphereSearchRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag")
	float DragZOffset = 6;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag")
	float ForwardOffsetBoxMulty = 12.f;

	FVector CharacterBounds;
	//====AudioTag====
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	FGameplayTag TagInteraction;
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	FGameplayTag TagJumpLand;
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	FGameplayTag TagDeath;

	UPROPERTY(BlueprintReadOnly, Category = "Component")
	UAudioComponent* AudioCompFootStep;
	UPROPERTY()
	UAudioComponent* AudioCompJump;
	UPROPERTY()
	UAudioComponent* AudioCompLand;
	UPROPERTY()
	UAudioComponent* AudioCompPickUp;
	UPROPERTY()
	UAudioComponent* AudioCompDrop;
	UPROPERTY()
	UAudioComponent* AudioCompPush;
	UPROPERTY()
	UAudioComponent* AudioCompInteraction;
	//====End AudioTag====

	//====SoundComponent====
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UTPP_SoundComponent> SoundComponent;
	//====End SoundComponent====
private:
	UPROPERTY(EditAnywhere, Category = "Pickup")
	float StartingMinWalkSpeed;
	UPROPERTY()
	FVector DragOffset;
	UPROPERTY()
	FRotator DragRotation;
	UPROPERTY()
	float SavedDragWorldZ = 0.f;
	UPROPERTY()
	FVector SavedDragHorizontalOffset = FVector::ZeroVector;
	bool bDraggedItemBlocked = false;

	UPROPERTY(EditAnywhere, Category="Movement")
	float BackwardSpeedMultiplier = 0.6f;

	UPROPERTY(EditDefaultsOnly)
	class UTPP_DeathComponent* DeathComp;
	
};
