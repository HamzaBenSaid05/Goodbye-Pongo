#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/TPP_PetBar.h"
#include "EventProxies/AICommsEvents/TPP_AICommsEventBase.h"
#include "AI/Struct/PongoStructs.h"
#include "Interfaces/Killable.h"
#include "Interfaces/AI/TPP_AIEmotionalStateObject.h"
#include "Interfaces/AI/TPP_AIPettable.h"
#include "TPP_PongoController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPongoChangedStateDelegate, ETPP_PongoState, NewState);

DECLARE_LOG_CATEGORY_EXTERN(LogPongoController, Log, All);

UCLASS()
class TPP_01_API ATPP_PongoController : public AAIController, public ITPP_AIPettable, public ITPP_AIEmotionalStateObject, public IKillable
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FPongoChangedStateDelegate OnPongoChangedState;

	void MovePongoToDestination(const FVector& Destination) const;
	
	UFUNCTION(BlueprintCallable, Category="Pongo|State")
	ETPP_PongoState GetPongoState() const;
	
	void FlushPendingEvents();

	bool GetIsMoving() const;

	virtual void Kill_Implementation(bool bSoftReset = false, const ECharacterDeath CharacterDeath = ECharacterDeath::PlayerDeath, const EDeathCause Cause = EDeathCause::KillBox) override;
	
	UFUNCTION()
	void LoadPongoStats();

private:
	// ------ AI EmotionalStateObject ------
	virtual void SetEmotionalStateObject_Implementation(AActor* EmotionalStateObject) override;
	virtual AActor* GetEmotionalStateObject() const override;
	// ------ End AI EmotionalStateObject ------

	// ------ AI Pettable ------
	// Called to verify if a agent can be pet
	virtual bool CanBePet() override;
	// Called when pet on agent is finished
	virtual void PetFinished_Implementation() override;
	// Called when pet on agent is started
	virtual void PetStart() override;
	// Called when pet on agent is unlocked
	virtual void UnlockPet_Implementation() override;
	// ------ End AI Pettable ------

	// ------ Set Player ------ 
	UFUNCTION()
	void SetPlayerBlackBoard(AActor* Player);
	// ------ End Set Player ------ 

	// ------ Behaviour Tree ------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|BehaviorTree", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UBehaviorTree> BehaviorTree;
	// ------ End Behaviour Tree ------

	// ------ Handle AI Event ------
	UPROPERTY()
	TArray<TWeakObjectPtr<const class UTPP_AICommsEventBase>> PendingEvents;
	int32 GetEventPriority(const class UTPP_AICommsEventBase* Event) const;
	void EnqueuePendingEvent(const class UTPP_AICommsEventBase* Event);
	bool CanApplyNow() const;
	UFUNCTION()
	void OnAICommEventTriggered(const class UTPP_AICommsEventBase* AICommsEvent);
	void ApplyAIEvent(const class UTPP_AICommsEventBase* AICommsEvent);
	UFUNCTION(BlueprintCallable)
	void ShouldUnlockStroll(bool bEnable) const;
	UFUNCTION(BlueprintCallable)
	void UnlockFearPlayer() const;
	UFUNCTION(BlueprintCallable)
	void ResetState() const;
	UFUNCTION(BlueprintCallable)
	void SetEmotionalState(const ETPP_PongoState NewState, AActor* EmotionalState) const;
	// ------ End Handle AI Event ------

	// ------ Blackboard Keys ------
	const FName PongoStateKeyName = "PongoState";
	const FName BondingStateKeyName = "BondingState";
	const FName PetStateKeyName = "PetState";

	const FName bIsMovingKeyName = "bIsMoving";
	const FName bCanStrollKeyName = "bCanStroll";
	const FName bIsInteractingKeyName = "bIsInteracting";
	const FName bIsComeHereCommandKeyName = "bIsComeHereCommand";
	const FName bIsGoThereCommandKeyName = "bIsGoThereCommand";
	const FName bIsBeingPetKeyName = "bIsBeingPet";
	const FName bCanBePetKeyName = "bCanBePet";
	const FName bIsFirstIdleDurationKeyName = "bIsFirstIdleDuration";
	const FName bCanBeStoppedKeyName = "bCanBeStopped";
	const FName bIsNoEmotionKeyName = "bIsNoEmotion";

	const FName TargetLocationKeyName = "TargetLocation";

	const FName EmotionalStateObjectKeyName = "EmotionalStateObject";
	const FName PlayerKeyName = "Player";
	const FName TargetActorKeyName = "TargetActor";

	const FName BondingValueKeyName = "BondingValue";
	const FName PongoJaneMinDistanceKeyName = "PongoJaneMinDistance";
	const FName FollowDurationKeyName = "FollowDuration";
	const FName MarginOfToleranceKeyName = "MarginOfTolerance";
	const FName FirstIdleDurationKeyName = "FirstIdleDuration";
	const FName StrollWaitTimeKeyName = "StrollWaitTime";
	const FName StrollRangeKeyName = "StrollRange";
	const FName IdleDurationBeforeDropObjectKeyName = "IdleDurationBeforeDropObject";
	const FName MaxIdleDurationKeyName = "MaxIdleDuration";
	const FName MinIdleDurationKeyName = "MinIdleDuration";
	const FName AnnoyedTravelDistanceKeyName = "AnnoyedTravelDistance";
	const FName EscapeFromPlayerDistanceMaxKeyName = "EscapeFromPlayerDistanceMax";
	const FName EscapeFromPlayerDistanceMinKeyName = "EscapeFromPlayerDistanceMin";
	UFUNCTION(BlueprintCallable, Category="Pongo|Interaction")
	void StopPlayerInteraction();
	// ------ End Blackboard Keys ------

	// ------ Bonding ------
	void ApplyBonding(float NewBondingValue);
	void UpdateBondingRanges(ETPP_PongoState CurrentState);
	void UpdateBondingValues(const FBondingValues* BoundingValues) const;
	void UpdateBondingColor(FLinearColor BoundingColor) const;
	template <typename T, typename TValueGetter>
	const T* FindBoundingValues(const TArray<T>& Array, float InValue, TValueGetter GetValue);
	const FBondingValues* FindBoundingValues(float InBondingValue) const;
	const FBondingColor* FindBondingColor(float InBondingValue) const;
	UPROPERTY(EditDefaultsOnly, Category="Bonding")
	TArray<FBondingData> BondingData
	{
		{ETPP_PongoState::Fear, 0.f, 40.f},
		{ETPP_PongoState::Curiosity, 30.f, 60.f,},
		{ETPP_PongoState::Trust, 50.f, 70.f,},
		{ETPP_PongoState::Rage, 70.f, 100.f,},
	};;
	UPROPERTY(EditDefaultsOnly, Category="Bonding|Default Values")
	FBondingValues DefaultValues;
	UPROPERTY(EditAnywhere)
	float CurrentBondingValue;
	UPROPERTY()
	FBondingData CurrentBondingData;
	void PrintBondingValues(const FBondingStep* InBondingStep) const;
	// ------ End Bonding ------

	// ------ Pet ------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI|Pet", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UTPP_PetBar> PetBarComponent;
	// ------ End Pet ------
	
	UFUNCTION()
	void SavePongoStats();

	// ------ Game Instance ------
	TWeakObjectPtr<class UTPP_GameInstance> GI;
	// ------ End Game Instance ------

protected:
	// ------ Pet ------
	UFUNCTION()
	void OnPetChangedState(EPetStatus NewState);
	UFUNCTION()
	void OnPetChangedSpeedMultiplierState(float NewSpeedMultiplier);
	// ------ End Pet ------

	// ------ Lifecycle ------
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	UPROPERTY()
	TObjectPtr<class ATPP_Pongo> PossessedPawn;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// ------ End Lifecycle ------

	//------ Audio ------
	UFUNCTION()
	void HandleStateChangedAudio(ETPP_PongoState NewState);
	//------ End Audio ------
};
