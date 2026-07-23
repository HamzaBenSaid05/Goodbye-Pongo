#include "AI/AIController/TPP_PongoController.h"

#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Pongo/TPP_Pongo.h"
#include "Components/AudioComponent.h"
#include "EventProxies/TPP_AICommsProxySubsystem.h"
#include "EventProxies/AICommsEvents/Bounding/TPP_AIComms_BondingChangeEvent.h"
#include "EventProxies/AICommsEvents/Drum/TPP_AIComms_MoveEvent.h"
#include "EventProxies/AICommsEvents/Drum/TPP_AIComms_GoThereEvent.h"
#include "EventProxies/AICommsEvents/Drum/TPP_AIComms_ComeHereEvent.h"
#include "EventProxies/AICommsEvents/Drum/TPP_AIComms_InteractEvent.h"
#include "EventProxies/AICommsEvents/Drum/TPP_AIComms_StopEvent.h"
#include "EventProxies/AICommsEvents/EmotionalState/TPP_AIComms_ChangeEmotionalStateEvent.h"
#include "EventProxies/AICommsEvents/EmotionalState/TPP_AIComms_ResetEvent.h"
#include "EventProxies/AICommsEvents/FearFromPlayer/TPP_AIComms_ChangeEscapeFromPlayerDistance.h"
#include "EventProxies/AICommsEvents/Petting/TPP_AIComms_OnPetEvent.h"
#include "Gameplay/GameInstances/TPP_GameInstance.h"
#include "Gameplay/GameModes/TPP_GameMode.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogPongoController);

void ATPP_PongoController::MovePongoToDestination(const FVector& Destination) const
{
	if (!PossessedPawn) return;

	FVector CurrentLocation = PossessedPawn->GetActorLocation();

	FVector Direction = (Destination - CurrentLocation).GetSafeNormal();

	PossessedPawn->AddMovementInput(Direction, 1.f);
}

ETPP_PongoState ATPP_PongoController::GetPongoState() const { return static_cast<ETPP_PongoState>(Blackboard->GetValueAsEnum(PongoStateKeyName)); }

void ATPP_PongoController::PrintBondingValues(const FBondingStep* InBondingStep) const
{
#if WITH_EDITOR

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
		                                 FString::Printf(
		                                                 TEXT("Closest Bounding Values found: %f"), InBondingStep->BondingValue));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
		                                 FString::Printf(
		                                                 TEXT("PongoJaneMinDistance: %f"),
		                                                 InBondingStep->BoundingValues.PongoJaneMinDistance));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
		                                 FString::Printf(
		                                                 TEXT("FollowDuration: %f"), InBondingStep->BoundingValues.FollowDuration));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
		                                 FString::Printf(
		                                                 TEXT("MarginOfTolerance: %f"),
		                                                 InBondingStep->BoundingValues.MarginOfTolerance));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
		                                 FString::Printf(
		                                                 TEXT("FirstIdleDuration: %f"),
		                                                 InBondingStep->BoundingValues.FirstIdleDuration));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
		                                 FString::Printf(
		                                                 TEXT("StrollRange: %f"), InBondingStep->BoundingValues.StrollRange));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
		                                 FString::Printf(
		                                                 TEXT("IdleDurationBeforeDropObject: %f"),
		                                                 InBondingStep->BoundingValues.IdleDurationBeforeDropObject));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
		                                 FString::Printf(
		                                                 TEXT("MaxIdleDuration: %f"),
		                                                 InBondingStep->BoundingValues.MaxIdleDuration));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
		                                 FString::Printf(
		                                                 TEXT("MinIdleDuration: %f"),
		                                                 InBondingStep->BoundingValues.MinIdleDuration));
	}

#endif
}

void ATPP_PongoController::LoadPongoStats()
{
	// Get Game Instance
	if (!GI.Get())
		GI = Cast<UTPP_GameInstance>(GetGameInstance());

	const UTPP_SaveGame* SaveGame = GI->GetSaveGameObject();
#if WITH_EDITOR
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
		                                 TEXT("Load Pongo Stats"));
	}
#endif
	// Read Pongo Value from save object
	if (SaveGame->bShouldFearFromPlayer)
	{
		UnlockFearPlayer();
		return;
	}

	Blackboard->SetValueAsEnum(PongoStateKeyName, static_cast<uint8>(ETPP_PongoState::Idle));
	OnPongoChangedState.Broadcast(ETPP_PongoState::Idle);

	// --- Bonding Value
	if (!Blackboard)
		return;
	CurrentBondingValue = SaveGame->BondingValue;
	Blackboard->SetValueAsFloat(BondingValueKeyName, CurrentBondingValue);
	// --- Bonding Data
	UpdateBondingRanges(SaveGame->BondingState);
	ApplyBonding(CurrentBondingValue);
	// --- Can Stroll
	if (SaveGame->bCanStroll) { Blackboard->SetValueAsBool(bCanStrollKeyName, true); }
	// --- Can be Pet
	if (SaveGame->bCanBePet)
		Blackboard->SetValueAsBool(bCanBePetKeyName, true);
}

void ATPP_PongoController::SavePongoStats()
{
	// Get Game Instance
	if (!GI.Get())
		GI = Cast<UTPP_GameInstance>(GetGameInstance());

	UTPP_SaveGame* SaveGame = GI->GetSaveGameObject();
#if WITH_EDITOR
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
		                                 TEXT("Save Pongo Stats"));
	}
#endif
	// Save Pongo Value
	if (!Blackboard)
		return;
	SaveGame->BondingValue = CurrentBondingValue;
	SaveGame->BondingState = static_cast<ETPP_PongoState>(Blackboard->GetValueAsEnum(BondingStateKeyName));
	SaveGame->bCanStroll = Blackboard->GetValueAsBool(bCanStrollKeyName);
	ETPP_PongoState CurrentPongoState = static_cast<ETPP_PongoState>(
		Blackboard->GetValueAsEnum(PongoStateKeyName));
	if (Blackboard->GetValueAsBool(bCanBePetKeyName))
		SaveGame->bCanBePet = true;
	UGameplayStatics::SaveGameToSlot(SaveGame, SaveGame->SaveSlotName, SaveGame->UserIndex);
}

void ATPP_PongoController::OnPetChangedState(EPetStatus NewState)
{
	Blackboard->SetValueAsEnum(PetStateKeyName, static_cast<uint8>(NewState));
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	if (NewState == EPetStatus::Annoyed)
	{
		Blackboard->SetValueAsFloat(AnnoyedTravelDistanceKeyName,
		                            PossessedPawn->GetPetBarComponent()->GetAnnoyedTravelDistance());
		Parameter.Int = 2;
		PossessedPawn->GetSoundComponent()->PlaySound(PossessedPawn->GetTagPet(), PossessedPawn->GetActorLocation(), PossessedPawn->GetAudioComponentPetPongo(),
		                                              Parameter);
		PossessedPawn->PlayReactionVFX(ETPP_PongoReactions::Annoyed);
	}
	else if (NewState == EPetStatus::Reassured)
	{
		PossessedPawn->PlayReactionVFX(ETPP_PongoReactions::Reassured);
		Parameter.Int = 0;
		PossessedPawn->GetSoundComponent()->PlaySound(PossessedPawn->GetTagPet(), PossessedPawn->GetActorLocation(), PossessedPawn->GetAudioComponentPetPongo(),
		                                              Parameter);
	}
	else if (NewState == EPetStatus::Unconcerned)
	{
		Parameter.Int = 1;
		PossessedPawn->GetSoundComponent()->PlaySound(PossessedPawn->GetTagPet(), PossessedPawn->GetActorLocation(), PossessedPawn->GetAudioComponentPetPongo(),
		                                              Parameter);
		PossessedPawn->PlayReactionVFX(ETPP_PongoReactions::Unconcerned);
	}
}

void ATPP_PongoController::OnPetChangedSpeedMultiplierState(float NewSpeedMultiplier) { PossessedPawn->SetSpeedMultiplier(NewSpeedMultiplier); }

void ATPP_PongoController::BeginPlay()
{
	Super::BeginPlay();
	if (UTPP_AICommsProxySubsystem* Sub = GetWorld()->GetSubsystem<UTPP_AICommsProxySubsystem>())
	{
		Sub->OnAICommEventTriggered.AddDynamic(
		                                       this,
		                                       &ATPP_PongoController::OnAICommEventTriggered);
	}
	OnPongoChangedState.AddDynamic(this, &ATPP_PongoController::HandleStateChangedAudio);
}

void ATPP_PongoController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (Cast<ATPP_Pongo>(InPawn))
	{
		PossessedPawn = Cast<ATPP_Pongo>(InPawn);
		RunBehaviorTree(BehaviorTree);
	}
	if (PossessedPawn->GetPetBarComponent())
	{
		PetBarComponent = PossessedPawn->GetPetBarComponent();
		PetBarComponent->OnPetChangedState.AddDynamic(this, &ATPP_PongoController::OnPetChangedState);
		PetBarComponent->OnPetChangedSpeedMultiplier.AddDynamic(
		                                                        this, &ATPP_PongoController::OnPetChangedSpeedMultiplierState);
	}
	if (ATPP_GameMode* GameMode = Cast<ATPP_GameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode->OnPlayerReady.AddDynamic(
		                                   this,
		                                   &ATPP_PongoController::SetPlayerBlackBoard);
	}
}

void ATPP_PongoController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	SavePongoStats();
}

int32 ATPP_PongoController::GetEventPriority(const UTPP_AICommsEventBase* Event) const
{
	if (!Event) return -1;

	// Highest: emotional state change to fear
	if (const auto* Emo = Cast<UTPP_AIComms_ChangeEmotionalStateEvent>(Event))
	{
		if (Emo->EmotionalState == ETPP_PongoState::Fear ||
			Emo->EmotionalState == ETPP_PongoState::FearPlayer) { return 1000; }
		return 200; // other emotional changes still above moves, but below fear
	}

	// Reset and change emotional state right after 
	if (Event->IsA(UTPP_AIComms_ResetEvent::StaticClass())) return 150;

	// Move event less priority than emotional state changes
	if (Event->IsA(UTPP_AIComms_GoThereEvent::StaticClass())) return 25;
	if (Event->IsA(UTPP_AIComms_ComeHereEvent::StaticClass())) return 25;

	// default
	return 0;
}

void ATPP_PongoController::EnqueuePendingEvent(const UTPP_AICommsEventBase* Event)
{
	if (!Event) return;

	// Avoid duplicates
	for (const auto& E : PendingEvents) { if (E.Get() == Event) return; }

	PendingEvents.Add(Event);

	// Reoder based on priority, higher priority events will be at the front of the queue
	PendingEvents.Sort([this](const TWeakObjectPtr<const UTPP_AICommsEventBase>& A,
	                          const TWeakObjectPtr<const UTPP_AICommsEventBase>& B)
	{
		return GetEventPriority(A.Get()) > GetEventPriority(B.Get());
	});
}

bool ATPP_PongoController::CanApplyNow() const
{
	if (!PossessedPawn) return false;

	const auto* Move = PossessedPawn->GetCharacterMovement();
	if (!Move) return false;

	return !(Move->IsFalling() || Move->IsFlying());
}

void ATPP_PongoController::FlushPendingEvents()
{
	if (!CanApplyNow()) return;

	while (PendingEvents.Num() > 0)
	{
#if WITH_EDITOR
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, "Landed: Flushing Pending Events"); }
#endif
		const UTPP_AICommsEventBase* Event = PendingEvents[0].Get();
		PendingEvents.RemoveAt(0);

		if (!Event) continue;

		// If this is a fear emotional event, drop pending move commands
		if (const auto* Emo = Cast<UTPP_AIComms_ChangeEmotionalStateEvent>(Event))
		{
			// if new emotional state is fear, remove all pending move and interact events
			if (Emo->EmotionalState == ETPP_PongoState::Fear ||
				Emo->EmotionalState == ETPP_PongoState::FearPlayer)
			{
				PendingEvents.RemoveAll([](const TWeakObjectPtr<const UTPP_AICommsEventBase>& E)
				{
					if (!E.IsValid()) return true;
					return E->IsA(UTPP_AIComms_GoThereEvent::StaticClass()) ||
						E->IsA(UTPP_AIComms_ComeHereEvent::StaticClass()) ||
						E->IsA(UTPP_AIComms_InteractEvent::StaticClass()) ||
						E->IsA(UTPP_AIComms_MoveEvent::StaticClass());
				});

				ApplyAIEvent(Event);
				// Since fear has highest priority, we can break after applying it
				break;
			}
		}

		ApplyAIEvent(Event);
	}
}

bool ATPP_PongoController::GetIsMoving() const
{
	if (!IsValid(Blackboard)) return false;
	return Blackboard->GetValueAsBool(bIsMovingKeyName);
}

void ATPP_PongoController::Kill_Implementation(bool bSoftReset, const ECharacterDeath CharacterDeath, const EDeathCause Cause)
{
	// TODO: Stop current movement, will be resumed on next replay, softReset case to be handled (world won't be reloaded, hence onpossess won't be called)
	if (bSoftReset)
	{
		ResetState();
	}
	else
	{
		if (UBrainComponent* Brain = GetBrainComponent())
		{
			Brain->StopLogic(TEXT("Pongo killed"));
		}
		UnPossess();
	}
}

void ATPP_PongoController::SetEmotionalStateObject_Implementation(AActor* EmotionalStateObject)
{
	if (!IsValid(Blackboard))
		return;
	if (!IsValid(EmotionalStateObject))
		return;
	Blackboard->ClearValue(EmotionalStateObjectKeyName);
	Blackboard->SetValueAsObject(EmotionalStateObjectKeyName, EmotionalStateObject);
#if WITH_EDITOR
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("New Emotional Object Set: %p"), EmotionalStateObject)); }
#endif
}

AActor* ATPP_PongoController::GetEmotionalStateObject() const
{
	if (!Blackboard)
		return nullptr;
	return Cast<AActor>(Blackboard->GetValueAsObject(EmotionalStateObjectKeyName));
}

bool ATPP_PongoController::CanBePet()
{
	if (!IsValid(Blackboard)) return false;
	const bool IsValidState = GetPongoState() == ETPP_PongoState::Idle || GetPongoState() == ETPP_PongoState::Stroll;
	return Blackboard->GetValueAsBool(bCanBePetKeyName) &&  IsValidState && !Blackboard->GetValueAsBool(bIsInteractingKeyName);
}

void ATPP_PongoController::PetFinished_Implementation() { if (PossessedPawn) { PossessedPawn->OnPetFinished.Broadcast(); } }

void ATPP_PongoController::PetStart() {}

void ATPP_PongoController::UnlockPet_Implementation()
{
	if (!IsValid(Blackboard)) return;
#if WITH_EDITOR
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Unlocked Pet"); }
#endif
	return Blackboard->SetValueAsBool(bCanBePetKeyName, true);
}

void ATPP_PongoController::SetPlayerBlackBoard(AActor* Player)
{
	if (Blackboard)
		Blackboard->SetValueAsObject(PlayerKeyName, Player);
}

void ATPP_PongoController::OnAICommEventTriggered(const UTPP_AICommsEventBase* AICommsEvent)
{
	const ETPP_PongoState CurrentState =
		static_cast<ETPP_PongoState>(Blackboard->GetValueAsEnum(PongoStateKeyName));

	const bool bIsAllowedEvent =
		AICommsEvent->IsA(UTPP_AIComms_ChangeEmotionalStateEvent::StaticClass()) ||
		AICommsEvent->IsA(UTPP_AIComms_ResetEvent::StaticClass()) ||
		AICommsEvent->IsA(UTPP_AIComms_BondingChangeEvent::StaticClass()) ||
		AICommsEvent->IsA(UTPP_AIComms_ChangeEscapeFromPlayerDistance::StaticClass());

	const bool bIsValidState =
		CurrentState == ETPP_PongoState::Idle ||
		CurrentState == ETPP_PongoState::Stroll ||
		CurrentState == ETPP_PongoState::Scripted;

	// Skip if event is not allowed AND state is not valid
	if (!bIsAllowedEvent && !bIsValidState) { return; }

	// If we are in the middle of a jump (cannot apply now)
	if (!CanApplyNow())
	{
		UE_LOG(LogPongoController, Log, TEXT("PendingEvent enqueue: %s"), *GetNameSafe(AICommsEvent));
		EnqueuePendingEvent(AICommsEvent);
		return;
	}

	UE_LOG(LogPongoController, Log, TEXT("Applying event: %s"), *GetNameSafe(AICommsEvent));

	ApplyAIEvent(AICommsEvent);
	FlushPendingEvents();
}

void ATPP_PongoController::StopPlayerInteraction()
{
	Blackboard->SetValueAsBool(bIsGoThereCommandKeyName, false);
	Blackboard->SetValueAsBool(bIsComeHereCommandKeyName, false);
	Blackboard->SetValueAsBool(bIsInteractingKeyName, false);
	Blackboard->SetValueAsBool(bIsMovingKeyName, false);
	StopMovement();
}

void ATPP_PongoController::ApplyBonding(float NewBondingValue)
{
	// Adding the new bonding increment, it can be negative or positive, and clamp it to the current min and max bonding values
	CurrentBondingValue = FMath::Clamp(CurrentBondingValue + NewBondingValue, CurrentBondingData.MinBondingValue,
	                                   CurrentBondingData.MaxBondingValue);
#if WITH_EDITOR
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
		                                 FString::Printf(TEXT("Bonding Change Received: %f"), CurrentBondingValue));
	}
#endif
	Blackboard->SetValueAsFloat(BondingValueKeyName, CurrentBondingValue);
	if (const FBondingValues* NewBoundingValues = FindBoundingValues(CurrentBondingValue))
	{
		UE_LOG(LogPongoController, Warning, TEXT("ApplyBonding -> UpdateBondingValues"));
		UpdateBondingValues(NewBoundingValues);
	}

	const FBondingColor* NewBondingColor = FindBondingColor(CurrentBondingValue);
	UE_LOG(LogPongoController, Warning, TEXT("ApplyBonding -> NewBondingColor: %s"),
	       NewBondingColor ? TEXT("VALID") : TEXT("NULL"));

	if (NewBondingColor)
	{
		UE_LOG(LogPongoController, Warning, TEXT("ApplyBonding -> UpdateBondingColor"));
		UpdateBondingColor(NewBondingColor->BoundingColor);
	}
}

void ATPP_PongoController::UpdateBondingRanges(ETPP_PongoState CurrentState)
{
	const int32 CurrentBondingIndex = BondingData.IndexOfByKey(CurrentBondingData.State);
	const int32 NewStateIndex = BondingData.IndexOfByKey(CurrentState);
	if (NewStateIndex == INDEX_NONE)
	{
		UE_LOG(LogPongoController, Error, TEXT("UpdateBondingRanges -> CurrentState %d not found in BondingData"),
		       static_cast<int32>(CurrentState));
		return;
	}
	if (CurrentBondingIndex != INDEX_NONE && CurrentBondingIndex > NewStateIndex)
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
			                                 FString::Printf(TEXT("New State Bonding Data is old")));
		}
#endif
		return;
	}
	const FBondingData* NewBondingDataPtr = BondingData.FindByKey(CurrentState);
	if (!NewBondingDataPtr)
	{
		UE_LOG(LogPongoController, Error, TEXT("UpdateBondingRanges -> No BondingData found for state %d"),
		       static_cast<int32>(CurrentState));
		return;
	}
	const FBondingData& NewBondingData = *NewBondingDataPtr;
	if (CurrentBondingValue < NewBondingData.MinBondingValue)
	{
		CurrentBondingValue = NewBondingData.MinBondingValue;
		Blackboard->SetValueAsFloat(BondingValueKeyName, CurrentBondingValue);
	}
	if (CurrentBondingValue > NewBondingData.MaxBondingValue)
	{
		CurrentBondingValue = NewBondingData.MaxBondingValue;
		Blackboard->SetValueAsFloat(BondingValueKeyName, CurrentBondingValue);
	}
	Blackboard->SetValueAsEnum(BondingStateKeyName, static_cast<uint8>(NewBondingData.State));

#if WITH_EDITOR
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
		                                 FString::Printf(TEXT("New State Bonding Data is %i"), NewBondingData.State));
	}
#endif
	CurrentBondingData = NewBondingData;

	if (const FBondingValues* NewBoundingValues = FindBoundingValues(CurrentBondingValue))
	{
		UE_LOG(LogPongoController, Warning, TEXT("ApplyBonding -> UpdateBondingValues"));
		UpdateBondingValues(NewBoundingValues);
	}

	const FBondingColor* NewBondingColor = FindBondingColor(CurrentBondingValue);
	UE_LOG(LogPongoController, Warning, TEXT("ApplyBonding -> NewBondingColor: %s"),
	       NewBondingColor ? TEXT("VALID") : TEXT("NULL"));

	if (NewBondingColor)
	{
		UE_LOG(LogPongoController, Warning, TEXT("ApplyBonding -> UpdateBondingColor"));
		UpdateBondingColor(NewBondingColor->BoundingColor);
	}
}

void ATPP_PongoController::UpdateBondingValues(const FBondingValues* BoundingValues) const
{
	if (BoundingValues->PongoJaneMinDistance > 0)
		Blackboard->SetValueAsFloat(PongoJaneMinDistanceKeyName, BoundingValues->PongoJaneMinDistance);
	else
		Blackboard->SetValueAsFloat(PongoJaneMinDistanceKeyName, DefaultValues.PongoJaneMinDistance);

	if (BoundingValues->FollowDuration > 0)
		Blackboard->SetValueAsFloat(FollowDurationKeyName, BoundingValues->FollowDuration);
	else
		Blackboard->SetValueAsFloat(FollowDurationKeyName, DefaultValues.FollowDuration);

	if (BoundingValues->MarginOfTolerance > 0)
		Blackboard->SetValueAsFloat(MarginOfToleranceKeyName, BoundingValues->MarginOfTolerance);
	else
		Blackboard->SetValueAsFloat(MarginOfToleranceKeyName, DefaultValues.MarginOfTolerance);

	if (BoundingValues->FirstIdleDuration > 0)
		Blackboard->SetValueAsFloat(FirstIdleDurationKeyName, BoundingValues->FirstIdleDuration);
	else
		Blackboard->SetValueAsFloat(FirstIdleDurationKeyName, DefaultValues.FirstIdleDuration);

	if (BoundingValues->StrollRange > 0)
		Blackboard->SetValueAsFloat(StrollRangeKeyName, BoundingValues->StrollRange);
	else
		Blackboard->SetValueAsFloat(StrollRangeKeyName, DefaultValues.StrollRange);

	if (BoundingValues->IdleDurationBeforeDropObject > 0)
		Blackboard->SetValueAsFloat(IdleDurationBeforeDropObjectKeyName, BoundingValues->IdleDurationBeforeDropObject);
	else
		Blackboard->SetValueAsFloat(IdleDurationBeforeDropObjectKeyName, DefaultValues.IdleDurationBeforeDropObject);

	if (BoundingValues->MaxIdleDuration > 0)
		Blackboard->SetValueAsFloat(MaxIdleDurationKeyName, BoundingValues->MaxIdleDuration);
	else
		Blackboard->SetValueAsFloat(MaxIdleDurationKeyName, DefaultValues.MaxIdleDuration);

	if (BoundingValues->MinIdleDuration > 0)
		Blackboard->SetValueAsFloat(MinIdleDurationKeyName, BoundingValues->MinIdleDuration);
	else
		Blackboard->SetValueAsFloat(MinIdleDurationKeyName, DefaultValues.MinIdleDuration);
}

void ATPP_PongoController::UpdateBondingColor(FLinearColor BoundingColor) const
{
	UE_LOG(LogPongoController, Warning, TEXT("UpdateBondingColor -> R:%f G:%f B:%f A:%f"),
	       BoundingColor.R, BoundingColor.G, BoundingColor.B, BoundingColor.A);
	PossessedPawn->StartChangingColor(BoundingColor,
	                                  static_cast<ETPP_PongoState>(Blackboard->GetValueAsEnum(PongoStateKeyName)) ==
	                                  ETPP_PongoState::Idle
		                                  ? true
		                                  : false, true);
}

template <typename T, typename TValueGetter>
const T* FindBestLowerOrEqual(const TArray<T>& Array, float InValue, TValueGetter GetValue)
{
	const T* BestItem = nullptr;
	float BestValue = -TNumericLimits<float>::Max();

	for (const T& Item : Array)
	{
		const float Value = GetValue(Item);

		if (Value <= InValue && Value > BestValue)
		{
			BestValue = Value;
			BestItem = &Item;
		}
	}

	return BestItem;
}

const FBondingValues* ATPP_PongoController::FindBoundingValues(float InBondingValue) const
{
	const FBondingStep* Step = FindBestLowerOrEqual(
	                                                CurrentBondingData.BondingSteps,
	                                                InBondingValue,
	                                                [](const FBondingStep& S) { return S.BondingValue; }
	                                               );

	if (Step)
	{
		PrintBondingValues(Step);
		return &Step->BoundingValues;
	}

	return nullptr;
}

const FBondingColor* ATPP_PongoController::FindBondingColor(float InBondingValue) const
{
	const FBondingColor* Result = FindBestLowerOrEqual(
	                                                   CurrentBondingData.BondingColorsData,
	                                                   InBondingValue,
	                                                   [](const FBondingColor& C) { return C.BondingValue; }
	                                                  );

#if WITH_EDITOR
	if (GEngine)
	{
		if (Result)
		{
			GEngine->AddOnScreenDebugMessage(
			                                 -1, 5.f, FColor::Green,
			                                 FString::Printf(TEXT("Found BondingColor at value: %f"), Result->BondingValue)
			                                );
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(
			                                 -1, 5.f, FColor::Red,
			                                 TEXT("FindBondingColor returned nullptr")
			                                );
		}
	}
#endif

	return Result;
}

void ATPP_PongoController::ApplyAIEvent(const class UTPP_AICommsEventBase* AICommsEvent)
{
	if (!AICommsEvent || !Blackboard) return;
	if (Cast<UTPP_AIComms_ComeHereEvent>(AICommsEvent))
	{
		// If we are interacting, ignore comand
		if (Blackboard->GetValueAsBool(bIsInteractingKeyName)) return;
#if WITH_EDITOR
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Come Here Received"); }
#endif
		OnPongoChangedState.Broadcast(ETPP_PongoState::Idle);
		Blackboard->SetValueAsBool(bIsMovingKeyName, true);
		Blackboard->SetValueAsBool(bIsGoThereCommandKeyName, false);
		Blackboard->SetValueAsBool(bIsComeHereCommandKeyName, true);
		Blackboard->SetValueAsObject(TargetActorKeyName, Blackboard->GetValueAsObject(PlayerKeyName));
		//Blackboard->SetValueAsVector(TargetLocationKeyName, ComeHereEvent->TargetLocation);
		return;
	}
	if (const UTPP_AIComms_GoThereEvent* GoThereEvent = Cast<UTPP_AIComms_GoThereEvent>(AICommsEvent))
	{
		// If we are interacting, ignore comand
		if (Blackboard->GetValueAsBool(bIsInteractingKeyName)) return;
		OnPongoChangedState.Broadcast(ETPP_PongoState::Idle);
#if WITH_EDITOR
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Go Here Received"); }
#endif
		Blackboard->SetValueAsBool(bIsComeHereCommandKeyName, false);
		Blackboard->SetValueAsBool(bIsMovingKeyName, true);
		Blackboard->SetValueAsBool(bIsGoThereCommandKeyName, true);
		Blackboard->SetValueAsVector(TargetLocationKeyName, GoThereEvent->TargetLocation);
		return;
	}
	if (Cast<UTPP_AIComms_StopEvent>(AICommsEvent))
	{
		// If we are interacting, ignore comand
		const ETPP_PongoState CurrenState = static_cast<ETPP_PongoState>(Blackboard->GetValueAsEnum(PongoStateKeyName));
		if (CurrenState != ETPP_PongoState::Idle && CurrenState != ETPP_PongoState::Stroll) return;
		if (PossessedPawn->GetIsDragging()) return;	
		if (Blackboard->GetValueAsBool(bIsInteractingKeyName)) return;
		//if (!Blackboard->GetValueAsBool(bCanBeStoppedKeyName)) return;
		Blackboard->SetValueAsBool(bIsFirstIdleDurationKeyName, true);
		//if (Blackboard->GetValueAsBool(bIsMovingKeyName))
		//{
#if WITH_EDITOR
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Stop Received"); }
#endif
		Blackboard->SetValueAsBool(bIsMovingKeyName, false);
		Blackboard->SetValueAsBool(bIsGoThereCommandKeyName, false);
		Blackboard->SetValueAsBool(bIsComeHereCommandKeyName, false);
		Blackboard->ClearValue(TargetActorKeyName);
		StopMovement();
		OnPongoChangedState.Broadcast(Blackboard->GetValueAsBool(bCanStrollKeyName) ? ETPP_PongoState::Stroll : ETPP_PongoState::Idle);
		return;
		//}
	}
	if (const UTPP_AIComms_ChangeEmotionalStateEvent* ChangeEmotionalStateEvent = Cast<
		UTPP_AIComms_ChangeEmotionalStateEvent>(AICommsEvent))
	{
		uint8 NewState = static_cast<uint8>(ChangeEmotionalStateEvent->EmotionalState);
		if (Blackboard->GetValueAsBool(bIsNoEmotionKeyName) &&
			(NewState == static_cast<uint8>(ETPP_PongoState::Fear) ||
			 NewState == static_cast<uint8>(ETPP_PongoState::Curiosity)))
			return;
#if WITH_EDITOR
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::White, "EmotionalStateEvent Received"); }
#endif
		if (
			Blackboard->GetValueAsEnum(PongoStateKeyName) == static_cast<uint8>(ETPP_PongoState::Fear) &&
			ChangeEmotionalStateEvent->EmotionalState != ETPP_PongoState::Fear
		)
			return;

		if (
			Blackboard->GetValueAsObject(EmotionalStateObjectKeyName) == ChangeEmotionalStateEvent->EmotionalStateObject &&
			ChangeEmotionalStateEvent->EmotionalState == ETPP_PongoState::Curiosity
		)
		{
#if WITH_EDITOR
			if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, "EmotionalStateEvent Skipped, curios about same object"); }
#endif
			return;
		}

		if (Blackboard->GetValueAsEnum(PongoStateKeyName) == NewState) { Blackboard->ClearValue(PongoStateKeyName); }
		Blackboard->SetValueAsBool(bIsBeingPetKeyName, false);
		Blackboard->SetValueAsEnum(PongoStateKeyName, NewState);
		OnPongoChangedState.Broadcast(ChangeEmotionalStateEvent->EmotionalState);
		if (ChangeEmotionalStateEvent->EmotionalStateObject)
		{
			Blackboard->SetValueAsObject(EmotionalStateObjectKeyName, ChangeEmotionalStateEvent->EmotionalStateObject);
			Blackboard->SetValueAsVector(TargetLocationKeyName,
			                             ChangeEmotionalStateEvent->EmotionalStateObject->GetActorLocation());
		}
		Blackboard->SetValueAsBool(bIsGoThereCommandKeyName, false);
		Blackboard->SetValueAsBool(bIsComeHereCommandKeyName, false);
		Blackboard->SetValueAsBool(bIsMovingKeyName, false);
		UpdateBondingRanges(ChangeEmotionalStateEvent->EmotionalState);
		//if (NewState == static_cast<uint8>(ETPP_PongoState::Curiosity) && !Blackboard->
		//	GetValueAsBool(bCanStrollKeyName))
		//{
		//	Blackboard->SetValueAsBool(bCanStrollKeyName, true);
		//	//ShouldUnlockStroll(true);
		//}
		return;
	}
	if (Cast<UTPP_AIComms_InteractEvent>(AICommsEvent))
	{
		if (PossessedPawn->GetIsDragging())
			return;
		OnPongoChangedState.Broadcast(ETPP_PongoState::Idle);
#if WITH_EDITOR
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Interact Received"); }
#endif
		Blackboard->SetValueAsBool(bIsGoThereCommandKeyName, false);
		Blackboard->SetValueAsBool(bIsComeHereCommandKeyName, false);
		Blackboard->SetValueAsBool(bIsMovingKeyName, false);
		Blackboard->SetValueAsBool(bIsInteractingKeyName, true);
		return;
	}
	if (const UTPP_AIComms_OnPetEvent* OnPetEvent = Cast<UTPP_AIComms_OnPetEvent>(AICommsEvent))
	{
		// If we are interacting, ignore comand
		if (Blackboard->GetValueAsBool(bIsInteractingKeyName)) return;
		if (PossessedPawn->GetIsDragging()) return;	

		ETPP_PongoState CurrenState = static_cast<ETPP_PongoState>(Blackboard->GetValueAsEnum(PongoStateKeyName));
		if (CurrenState == ETPP_PongoState::Scripted)
		{
			Blackboard->SetValueAsEnum(PongoStateKeyName, static_cast<uint8>(ETPP_PongoState::Idle));
			OnPongoChangedState.Broadcast(ETPP_PongoState::Idle);
			UpdateBondingRanges(ETPP_PongoState::Fear);
		}
		if (CurrenState != ETPP_PongoState::Idle &&
			CurrenState != ETPP_PongoState::Scripted &&
			CurrenState != ETPP_PongoState::Stroll)
			return;

		OnPongoChangedState.Broadcast(ETPP_PongoState::Idle);
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Petting Received"); }
		if (!PetBarComponent)
		{
			UE_LOG(LogPongoController, Error, TEXT("PetBarComponent is NULL in ApplyAIEvent on %s"), *GetName());
			return;
		}
		PetBarComponent->AddPetValue(OnPetEvent->PetPercentValue);
		Blackboard->SetValueAsBool(bIsBeingPetKeyName, true);
		return;
	}
	if (const UTPP_AIComms_BondingChangeEvent* BondingChangeEvent = Cast<UTPP_AIComms_BondingChangeEvent>(AICommsEvent))
	{
		UpdateBondingRanges(ETPP_PongoState::Fear);
		ApplyBonding(BondingChangeEvent->BondingValue);
		return;
	}
	if (Cast<UTPP_AIComms_ResetEvent>(AICommsEvent))
	{
		StopPlayerInteraction();
		if (Blackboard->GetValueAsBool(bCanStrollKeyName))
		{
			Blackboard->SetValueAsEnum(PongoStateKeyName, static_cast<uint8>(ETPP_PongoState::Stroll));
			OnPongoChangedState.Broadcast(ETPP_PongoState::Stroll);
		}
		else
		{
			Blackboard->SetValueAsEnum(PongoStateKeyName, static_cast<uint8>(ETPP_PongoState::Idle));
			OnPongoChangedState.Broadcast(ETPP_PongoState::Idle);
		}
	}
	if (const UTPP_AIComms_ChangeEscapeFromPlayerDistance* ChangeEscapeFromPlayerDistanceEvent = Cast<
		UTPP_AIComms_ChangeEscapeFromPlayerDistance>(AICommsEvent))
	{
		Blackboard->SetValueAsFloat(EscapeFromPlayerDistanceMaxKeyName,
		                            ChangeEscapeFromPlayerDistanceEvent->EscapeFromPlayerDistanceMax);
		Blackboard->SetValueAsFloat(EscapeFromPlayerDistanceMinKeyName,
		                            ChangeEscapeFromPlayerDistanceEvent->EscapeFromPlayerDistanceMin);
	}
}

void ATPP_PongoController::ShouldUnlockStroll(bool bEnable) const
{
	Blackboard->SetValueAsBool(bCanStrollKeyName, bEnable);
	if (bEnable)
	{
		Blackboard->SetValueAsEnum(PongoStateKeyName, static_cast<uint8>(ETPP_PongoState::Stroll));
		OnPongoChangedState.Broadcast(ETPP_PongoState::Stroll);
	}
	else
	{
		Blackboard->SetValueAsEnum(PongoStateKeyName, static_cast<uint8>(ETPP_PongoState::Idle));
		OnPongoChangedState.Broadcast(ETPP_PongoState::Idle);
	}
#if WITH_EDITOR
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Unlock Stroll Received"); }
#endif
}

void ATPP_PongoController::UnlockFearPlayer() const
{
	Blackboard->SetValueAsEnum(PongoStateKeyName, static_cast<uint8>(ETPP_PongoState::FearPlayer));
	OnPongoChangedState.Broadcast(ETPP_PongoState::FearPlayer);
}

void ATPP_PongoController::ResetState() const
{
	if (UTPP_AICommsProxySubsystem* Sub = GetWorld()->GetSubsystem<UTPP_AICommsProxySubsystem>())
	{
		UTPP_AIComms_ResetEvent* ResetEvent = NewObject<UTPP_AIComms_ResetEvent>();
		UE_LOG(LogPongoController, Log, TEXT("ResetEvent"));
		Sub->NotifyAICommEvent(ResetEvent);
		OnPongoChangedState.Broadcast(ETPP_PongoState::Idle);
	}
}

void ATPP_PongoController::SetEmotionalState(const ETPP_PongoState NewState, AActor* EmotionalState) const
{
	if (UTPP_AICommsProxySubsystem* Sub = GetWorld()->GetSubsystem<UTPP_AICommsProxySubsystem>())
	{
		UTPP_AIComms_ChangeEmotionalStateEvent* ChangeEmotionalStateEvent = NewObject<
			UTPP_AIComms_ChangeEmotionalStateEvent>();
		ChangeEmotionalStateEvent->Init(NewState, EmotionalState);
		Sub->NotifyAICommEvent(ChangeEmotionalStateEvent);
		OnPongoChangedState.Broadcast(NewState);
	}
}

void ATPP_PongoController::HandleStateChangedAudio(ETPP_PongoState NewState)
{
	if (NewState == ETPP_PongoState::Stroll)
		PossessedPawn->GetSoundComponent()->PlaySound(PossessedPawn->GetTagStroll(), PossessedPawn->GetActorLocation(),
		                                              PossessedPawn->GetAudioComponentState());
}
