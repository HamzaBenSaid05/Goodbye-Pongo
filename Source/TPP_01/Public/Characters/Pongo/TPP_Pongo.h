#pragma once

#include "CoreMinimal.h"
#include "AI/AIController/TPP_PongoController.h"
#include "Characters/TPP_CharacterBase.h"
#include "Components/TPP_PetBar.h"
#include "AI/Enum/TPP_PongoEnum.h"
#include "Interfaces/AI/TPP_AIAction.h"
#include "Interfaces/AI/TPP_AITaggable.h"
#include "TPP_Pongo.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPongoPetFinished);

UCLASS()
class TPP_01_API ATPP_Pongo : public ATPP_CharacterBase, public ITPP_AIAction, public ITPP_AITaggable
{
	GENERATED_BODY()

public:
	// ------ Constructor ------ 
	ATPP_Pongo();
	// ------ End Constructor ------ 

	// ------ Getter Controller ------
	UFUNCTION(BlueprintCallable, Category="AI|Controller")
	ATPP_PongoController* GetPongoController() const { return ControllerRef; }
	// ------ End Getter Controller  ------
	
	// ------ Interact ------
	void EndDragAction() override;
	bool bCanPongoInteract;
	// ------ End Interact ------ 
	
	
	// ------ Speed ------
	void SetSpeedMultiplier(float NewMultiplier)
	{
		SpeedMultiplier = NewMultiplier;
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
			                                 FString::Printf(TEXT("Speed Multiplier: %f"), SpeedMultiplier));
		}
#endif
	}
	// ------ End Speed ------

	// ------ Pet ------
	UFUNCTION(BlueprintCallable, Category="AI|Pet")
	UTPP_PetBar* GetPetBarComponent() const { return PetBarComponent; }
	UPROPERTY(BlueprintCallable, Category="AI|Pet")
	FPongoPetFinished OnPetFinished;
	// ------ End Pet ------

	// ------ Color ------
	UFUNCTION(BlueprintCallable, Category = "Color")
	void StartChangingColor(FLinearColor NewColor, bool bIsIdle = false, bool bIsBonding = false);
	// ------ End Color ------
 
	// ------ VFX ------
	UFUNCTION(BlueprintImplementableEvent, Category="VFX")
	void PlayReactionVFX(ETPP_PongoReactions Reaction);
	// ------ End VFX ------
	// ------ Audio ------
	UFUNCTION()
	FGameplayTag GetTagPet() const  {return TagPet;}
	UFUNCTION()
	UAudioComponent*& GetAudioComponentPetPongo()  {return AudioCompPet;}
	UFUNCTION()
	UAudioComponent*& GetAudioComponentState()  {return AudioCompState;}
	UFUNCTION()
	FGameplayTag GetTagStroll() const  {return TagStroll;}
	// ------ End Audio ------
private:
	// ------ Drag ------ 
	virtual void GetBoxTraceIgnoredActors(TArray<AActor*>& OutIgnored) override;
	// ------ End Drag ------ 

	// ------ AI Taggable ------
	virtual void AddGameplayTag_Implementation(FGameplayTag Tag) override;
	virtual bool HasGameplayTag_Implementation(FGameplayTag Tag) override { return OwnedTags.HasTag(Tag); }
	virtual void RemoveGameplayTag_Implementation(FGameplayTag Tag) override { OwnedTags.RemoveTag(Tag); }
	// ------ End AI Taggable ------
	
	// ------ AI Action ------
	// Called when is on Smart Nav Link
	virtual void Jump(const FVector& EndPos, const float& DestinationZOffset) override;
	// Called when speed need to be changed
	virtual void SetSpeedBasedOnState(uint8 State) override;
	// Called when agent reset color to default
	virtual void ResetColor() override;
	virtual void SetColor_Implementation(FLinearColor NewColor) override;
	// Called to toggle player collision
	virtual void TogglePlayerCollision_Implementation(bool bEnable) override;
	//
	// ------ End AI Action ------
	
	// ------ Jump ------
	virtual void Landed(const FHitResult& Hit) override;
	// Jump arc factor in range [0.0 - 1.0]
	UPROPERTY(EditAnywhere, Category = "AI|Jump",
		meta = (ClampMin = "0.4", ClampMax = "0.6", ToolTip = "0.5: Balanced, >0.5: Flatter/Faster, <0.5: Higher/Slower"
		))
	float JumpArc = 0.5f;
	// ------ End Jump ------

	// ------ Gameplay Tags Container ------
	UPROPERTY(BlueprintReadOnly, Category = "AI|GameplayTags", meta=(AllowPrivateAccess="true"))
	FGameplayTagContainer OwnedTags;
	// ------ End Gameplay Tags Container ------
	
	// ------ Speed ------
	float SpeedMultiplier = 1.f;
	float DefaultMovementSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Speed", meta = (AllowPrivateAccess = "true"))
	float BaseMovementSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Speed", meta = (AllowPrivateAccess = "true"))
	float ScaredMovementSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Speed", meta = (AllowPrivateAccess = "true"))
	float ScaredFromPlayerMovementSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Speed", meta = (AllowPrivateAccess = "true"))
	float CuriousMovementSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Speed", meta = (AllowPrivateAccess = "true"))
	float RagingMovementSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Speed", meta = (AllowPrivateAccess = "true"))
	float TrustingMovementSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Speed", meta = (AllowPrivateAccess = "true"))
	float ScriptedMovementSpeed;
	// Speed Setter
	UFUNCTION(Blueprintable, Category = "Speed")
	void SetCharacterSpeed(const float NewSpeed) const;
	UFUNCTION(BlueprintCallable, Category = "Speed")
	void SetCharacterSpeedBasedOnState(ETPP_PongoState State);
	// ------ End Speed ------

	// ------ Color ------
	FLinearColor StartColor;
	FLinearColor CurrentColor;
	FLinearColor TargetColor;
	float ElapsedTime = 0.0f;
	bool bIsLerpingColor = false;
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* Fur_MaterialInst;
	// Color States
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Color|Lerp", meta = (AllowPrivateAccess = "true"))
	float LerpDuration = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Color", meta = (AllowPrivateAccess = "true"))
	FLinearColor BaseMovementColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Color", meta = (AllowPrivateAccess = "true"))
	FLinearColor ScaredMovementColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Color", meta = (AllowPrivateAccess = "true"))
	FLinearColor ScaredFromPlayerMovementColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Color", meta = (AllowPrivateAccess = "true"))
	FLinearColor CuriousMovementColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Color", meta = (AllowPrivateAccess = "true"))
	FLinearColor RagingMovementColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Color", meta = (AllowPrivateAccess = "true"))
	FLinearColor TrustingMovementColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Color", meta = (AllowPrivateAccess = "true"))
	FLinearColor StrollMovementColor;
	UFUNCTION(BlueprintCallable, Category = "Color")
	void SetCharacterColorBasedOnState(ETPP_PongoState State);
	UPROPERTY(Transient)
	FName SocketToUse;
	UPROPERTY(EditDefaultsOnly, Category = "AI|Color", meta = (ToolTip = "The socket from which the color will start changing, used to set the center of the radial effect in the material"))
	TObjectPtr<class UTPP_FurTransitionSocketMap> FurTransitionDA;
	// ------ End Color ------

	// ------ Pet ------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI|Pet", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UTPP_PetBar> PetBarComponent;
	// ------ End Pet ------
	
	// ------ VFX ------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX", meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UTPP_PongoReactions> ReactionsDA;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX", meta=(AllowPrivateAccess="true"))
	class UNiagaraComponent* ReactionsVFX;
	UPROPERTY(BlueprintReadWrite, Transient, meta=(AllowPrivateAccess="true"))
	bool bIsPlayingReactionVFX;
	// ------ End VFX ------

	// ------ Audio ------
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	FGameplayTag TagEmotional;
	UPROPERTY(EditAnywhere,  Category = "Audio Tag")
	FGameplayTag TagPet;
	UPROPERTY(EditAnywhere,Category = "Audio Tag")
	FGameplayTag TagStroll;
	
	UPROPERTY()
	UAudioComponent* AudioCompState;
	UPROPERTY()
	UAudioComponent* AudioCompPet;
	
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	FTimerHandle CuriosityTimerHandle;
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	float CuriosityCooldown = 7.0f;
	UFUNCTION(BlueprintCallable, Category = "Audio Tag")
	void CheckCuriosityCooldown();
	// ------ End Audio ------
	
	// ------ Interactable -----
	TWeakObjectPtr<ATPP_InteractableItem> PendingJumpItem;
	// ------ End Interactable -----
	
protected:
	// ------ Lifecycle ------
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	// On Possessed by
	virtual void PossessedBy(AController* NewController) override;
	
	UPROPERTY(Transient)
	TObjectPtr<ATPP_PongoController> ControllerRef;
	// ------ End Lifecycle ------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* FurMesh;

	virtual void OnItemDetached(ATPP_InteractableItem* Item) override;
};
