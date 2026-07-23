#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "Enums/TPP_DeathInfo.h"
#include "Interfaces/Killable.h"
#include "Interfaces/Resettable.h"
#include "Kismet/GameplayStatics.h"
#include "TPP_PlayerController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDrumActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDrumDeactivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnComeHereActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGoThereActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGoThereCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPauseMenuOpen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPauseMenuClosed);

UCLASS()
class TPP_01_API ATPP_PlayerController : public APlayerController, public IKillable, public IResettable
{
	GENERATED_BODY()

public:
	void RestertLevel() {
		UGameplayStatics::OpenLevel(GetWorld(), FName("LogicLevel"));
	}

	UPROPERTY(BlueprintAssignable, Category = "Drum Events")
	FOnDrumActivated OnDrumActivated;
	UPROPERTY(BlueprintAssignable, Category = "Drum Events")
	FOnDrumDeactivated OnDrumDeactivated;
	UPROPERTY(BlueprintAssignable, Category = "Drum Events")
	FOnComeHereActivated OnComeHereActivated;
	UPROPERTY(BlueprintAssignable, Category = "Drum Events")
	FOnGoThereActivated OnGoThereActivated;
	UPROPERTY(BlueprintAssignable, Category = "Drum Events")
	FOnGoThereCompleted OnGoThereCompleted;
	UPROPERTY(BlueprintAssignable, Category = "Drum Events")
	FOnInteractActivated OnInteractActivated;
	UPROPERTY(BlueprintAssignable, Category = "Drum Events")
	FOnInteractActivated OnStopActivated;
	UPROPERTY(BlueprintAssignable,BlueprintCallable, Category = "Drum Events")
	FOnPauseMenuOpen OnPauseMenuOpen;
	UPROPERTY(BlueprintAssignable,BlueprintCallable, Category = "Drum Events")
    FOnPauseMenuOpen OnPauseMenuClosed;
	UFUNCTION(BlueprintCallable)
	void SetupDrumInput();
	UFUNCTION(BlueprintCallable)
	void ToggleLocomotionMappingContext(bool bEnable);
	UFUNCTION(BlueprintCallable)
	void ToggleDrumMappingContext(bool bEnable);


	virtual void Kill_Implementation(bool bSoftReset = false, const ECharacterDeath CharacterDeath = ECharacterDeath::PlayerDeath, const EDeathCause Cause = EDeathCause::KillBox) override;
	virtual void HandleReset() override;
protected:
	// ===== Lifecycle =====
	UFUNCTION()
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	// ===== UI =====
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void HandleOptionMenu();

private:
	// ===== INPUT =====
	// ===== INPUT CHARACTER =====
	UFUNCTION()
	virtual void SetupInputComponent() override;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	const TSoftObjectPtr<UInputMappingContext> LocomotionContext;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	const TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	const TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	const TObjectPtr<UInputAction> InteractAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	const TObjectPtr<UInputAction> CrouchAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	const TObjectPtr<UInputAction> PettingAction;
	
	// ===== INPUT UI =====
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	const TObjectPtr<UInputAction> OptionMenuAction;

	// ===== INPUT DRUM =====
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	const TSoftObjectPtr<UInputMappingContext> DrumContext;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	const TObjectPtr<UInputAction> DrumToggle;

	// ===== INPUT ACTION DRUM =====
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	const TSoftObjectPtr<UInputMappingContext> DrumActionContext;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	const TObjectPtr<UInputAction> ComeHereAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	const TObjectPtr<UInputAction> PongoInteractAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	const TObjectPtr<UInputAction> GoThereAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	const TObjectPtr<UInputAction> StopAction;


	// ===== MOVEMENT =====
	void Move(const FInputActionValue& Value);
	void Jump();
	void StopJumping();
	void Crouch();
	void Uncrouch();

	//===== Interact =====
	void Interact();

	// ===== DRUM =====
	void DrumActive();
	void DrumActiveStarted();
	bool CheckDrumActive() const;
	void DrumInactive();
	void GoThereStarted();
	void GoThere();
	void GoThereCompleted();
	void PongoInteract();
	void ComeHere();
	void Stop();

	//===== UI =====
	void OpenOptionMenu();
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UTPP_BaseWidget> OptionMenuWidget;

	UPROPERTY()
	TObjectPtr<class ATPP_PlayerCharacter> PlayerCharacter;
	UPROPERTY()
	TObjectPtr<class ULocalPlayer> LocalPlayer;
	UPROPERTY()
	TObjectPtr<class UEnhancedInputLocalPlayerSubsystem> InputSubsystem;

	uint8 bDrumStarted : 1 = false;
};
