#include "Gameplay/GameModes/TPP_GameMode.h"

#include "EngineUtils.h"
#include "Camera/TPP_Camera.h"
#include "Characters/Pongo/TPP_Pongo.h"
#include "Characters/Player/TPP_PlayerCharacter.h"
#include "Components/TPP_DeathComponent.h"
#include "CutScene/TPP_LevelSequencePlayer.h"
#include "EventProxies/TPP_RoomsProxySubsystem.h"
#include "EventProxies/RoomsEvents/TPP_PongoChangedRoomEvent.h"
#include "GameFramework/PlayerStart.h"
#include "Gameplay/GameInstances/TPP_GameInstance.h"
#include "Gameplay/PongoPlayerStart/TPP_PongoPlayerStart.h"
#include "Interfaces/Resettable.h"
#include "Kismet/GameplayStatics.h"
#include "RoomManagement/TPP_RoomStreamingSubsystem.h"
#include "UI/HUD/TPP_HUDGame.h"
#include "VFX/PongoInteractables/TPP_InteractableHighlightingSubsystem.h"

ATPP_GameMode::ATPP_GameMode()
{
	DefaultPawnClass = nullptr;
}

void ATPP_GameMode::BeginPlay()
{
	Super::BeginPlay();

	// Initialize the room streaming subsystem based on the saved room index
	if (!GI.Get())
		GI = GetGameInstance<UTPP_GameInstance>();

	if (UTPP_RoomsProxySubsystem* RoomProxySubsystem = GetWorld()->GetSubsystem<UTPP_RoomsProxySubsystem>())
	{
		// RoomProxySubsystem->OnRoomEvent.AddDynamic(this, &ATPP_GameMode::PongoChangedRoom);
		RoomProxySubsystem->OnRoomEvent.AddDynamic(this, &ATPP_GameMode::PongoChangedRoom);
	}

	// Get the room streaming subsystem and initialize it with the saved room index
	RoomStreamingSubsystem = GetWorld()->GetSubsystem<UTPP_RoomStreamingSubsystem>();
	if (RoomStreamingSubsystem.IsValid())
	{
		RoomStreamingSubsystem->JaneSpawnRoomsAllowList = JaneSpawnRoomsAllowList;
		int32 RoomIndexToUse = GI->GetRoomIndexToLoad();
#if WITH_EDITOR
		if (SpawnRoomIndex != INDEX_NONE)
		{
			RoomIndexToUse = SpawnRoomIndex;
			GI->SetCurrentRoomIndex(RoomIndexToUse);
		}
#endif
		RoomStreamingSubsystem->PongoStartRoomIndex = PongoStartRoomIndex;
		RoomStreamingSubsystem->InitializeFromSave(RoomIndexToUse);
		RoomStreamingSubsystem->OnSpawnRoomReady.AddDynamic(this, &ATPP_GameMode::SpawnPlayer);
		RoomStreamingSubsystem->OnNewRoomReady.AddDynamic(this, &ATPP_GameMode::HandleNewRoomsLoaded);
	}

	// Set the player reference
	PlayerControllerRef = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (AActor* PongoActor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPP_Pongo::StaticClass()))
	{
		PongoRef = Cast<ATPP_Pongo>(PongoActor);
		StartingOPongoLocation = PongoRef->GetActorLocation();
		PongoRef->GetDeathComp()->OnDead.AddDynamic(this, &ATPP_GameMode::HandleGameOver);

		if (UTPP_InteractableHighlightingSubsystem* InteractableHighlightingSubsystem = GetWorld()->GetSubsystem<
			UTPP_InteractableHighlightingSubsystem>())
		{
			InteractableHighlightingSubsystem->SetMaxDistance(MaxDistance);
			InteractableHighlightingSubsystem->SetPongoRef(PongoRef.Get());
			InteractableHighlightingSubsystem->StartScanning(ItemScanningInterval);
		}
	}
	LevelSequencePlayer = Cast<ATPP_LevelSequencePlayer>
	(
		UGameplayStatics::GetActorOfClass(GetWorld(), ATPP_LevelSequencePlayer::StaticClass()));
	if (!LevelSequencePlayer.IsValid()) return;

	// Get Game Instance
	if (!GI.Get())
		GI = GetGameInstance<UTPP_GameInstance>();

	const UTPP_SaveGame* SaveGame = GI->GetSaveGameObject();
	if (!SaveGame->bHasSeenFirstCutScene)
	{
		if (ATPP_HUDGame* GameHud = Cast<ATPP_HUDGame>
		(
			GetWorld()->GetFirstPlayerController()->GetHUD()))
		{
			GameHud->OnHUDReady.AddDynamic(this, &ATPP_GameMode::OnHUDReady);
		}
	}
}

void ATPP_GameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::Type::LevelTransition)
	{
		if (!GI.Get())
			GI = GetGameInstance<UTPP_GameInstance>();

		GI->SetCurrentRoomIndex(RoomStreamingSubsystem->GetCurrentRoomIndex());
	}
}

void ATPP_GameMode::HandlePlayerChangedRoom(AActor* NewCamera, float InBlendTime)
{
	PlayerControllerRef->SetViewTargetWithBlend(NewCamera, InBlendTime, VTBlend_EaseInOut, 3, true);

	if
	(
		ULevelStreaming* Level = ULevelStreaming::FindStreamingLevel(NewCamera->GetLevel())
	)
	{
		if (RoomStreamingSubsystem.IsValid())
			RoomStreamingSubsystem.Get()->NotifyRoomEntered(Level);
	}
}

void ATPP_GameMode::HandleNewRoomsLoaded()
{
	TArray<AActor*> CameraActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATPP_Camera::StaticClass(), CameraActors);

	for (const auto CameraActor : CameraActors)
	{
		ATPP_Camera* Camera = CastChecked<ATPP_Camera>(CameraActor);

		if (Camera->GetPlayerRef()) continue;

		// Remove previous binding to avoid duplicate delegate errors
		if (Camera->OnCameraChange.IsAlreadyBound(this, &ATPP_GameMode::HandlePlayerChangedRoom))
			Camera->OnCameraChange.RemoveDynamic(this, &ATPP_GameMode::HandlePlayerChangedRoom);

		// Add binding
		Camera->OnCameraChange.AddDynamic(this, &ATPP_GameMode::HandlePlayerChangedRoom);

		if (PlayerRef.IsValid())
			Camera->SetPlayerRef(PlayerRef.Get());
	}
}

void ATPP_GameMode::SpawnPlayer(AActor* PlayerStartToUse, AActor* PongoPlayerStart)
{
	if (!PlayerControllerRef.IsValid() || !PlayerStartToUse || !CharacterClass)
	{
		return;
	}

	DefaultPawnClass = CharacterClass;

	RestartPlayerAtPlayerStart(PlayerControllerRef.Get(), PlayerStartToUse);

	// Finding the relative camera and activating it
	for (TActorIterator<ATPP_Camera> It(GetWorld()); It; ++It)
	{
		if (It->GetLevel()->GetOuter() == PlayerStartToUse->GetLevel()->GetOuter())
		{
			It->bIsInUse = true;
			HandlePlayerChangedRoom(*It, 0);
			break;
		}
	}

	PlayerRef = Cast<ATPP_PlayerCharacter>(PlayerControllerRef.Get()->GetCharacter());
	if (!PlayerRef.Get())
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerRef is NULL after spawn"));
		return;
	}
	PlayerRef->GetDeathComp()->OnDead.AddDynamic(this, &ATPP_GameMode::HandleGameOver);

	const FVector PongoSpawnPos = PongoPlayerStart ? PongoPlayerStart->GetActorLocation() : StartingOPongoLocation;
	PongoRef->SetActorLocation(PongoSpawnPos, false, nullptr, ETeleportType::ResetPhysics);
	if (PongoPlayerStart && PongoPlayerStart->Implements<UTPP_PongoPlayerStart>())
	{
		UTPP_SaveGame* SaveGame = GI->GetSaveGameObject();
		SaveGame->bShouldFearFromPlayer = ITPP_PongoPlayerStart::Execute_GetShouldEscapeFromPlayer(PongoPlayerStart);
		UGameplayStatics::SaveGameToSlot(SaveGame, SaveGame->SaveSlotName, SaveGame->UserIndex);
	}
	PongoRef->GetPongoController()->LoadPongoStats();
	OnPlayerReady.Broadcast(PlayerRef.Get());
	OnDeactivateLoadingScreen.Broadcast();
}

void ATPP_GameMode::PongoChangedRoom(const class UTPP_RoomEventBase* RoomEvent)
{
	if (RoomEvent->IsA(UTPP_PongoChangedRoomEvent::StaticClass()))
	{
		const auto RoomEvt = Cast<UTPP_PongoChangedRoomEvent>(RoomEvent);
		RoomStreamingSubsystem->UpdateCompanionRoom(RoomEvt->RoomActor);
	}
}

void ATPP_GameMode::GetCharacters(TArray<AActor*>& InArray) const
{
	InArray.AddUnique(PlayerRef.Get());
	InArray.AddUnique(PongoRef.Get());
}

void ATPP_GameMode::Restart()
{
	if (bShouldSoftReset)
	{
		// Soft reset: respawn player in the same room, keeping the same world state by simply tping him
		const ULevelStreaming* Room = RoomStreamingSubsystem->GetRooms()[RoomStreamingSubsystem->GetCurrentRoomIndex()];

		TArray<AActor*> PlayerStarts;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

		for (const AActor* PlayerStart : PlayerStarts)
		{
			if (PlayerStart->GetLevel()->GetOuter() == Room->GetLoadedLevel()->GetOuter())
			{
				if (PlayerStart->ActorHasTag("PongoPlayerStart"))
				{
					PongoRef->SetActorLocation(PlayerStart->GetActorLocation());
				}
				else
				{
					GetPlayerCharacterRef()->SetActorLocation(PlayerStart->GetActorLocation());
				}
			}
		}

		PlayerRef->GetDeathComp()->bIsDying = false;
		PongoRef->GetDeathComp()->bIsDying = false;

		if (IResettable* ResettablePlayer = Cast<IResettable>(PlayerControllerRef))
		{
			ResettablePlayer->HandleReset();
		}

		if (IResettable* ResettablePongo = Cast<IResettable>(PongoRef->GetController()))
		{
			ResettablePongo->HandleReset();
		}
	}
	else
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), MoveTemp(MainLevel));
	}

	bShouldSoftReset = false;
}

void ATPP_GameMode::HandleGameOver(FDeathInfo DeathInfo)
{
	bShouldSoftReset = DeathInfo.bShouldSoftReset;

	OnGameOver.Broadcast(DeathInfo.CharacterDeath, DeathInfo.DeathCause);
}

void ATPP_GameMode::OnHUDReady()
{
	if (ULevelSequence* LevelSequence = LevelSequenceToPlayOnStartSoft.LoadSynchronous())
	{
		LevelSequencePlayer->PlaySequence(LevelSequence);
		UTPP_SaveGame* SaveGame = GI->GetSaveGameObject();
		SaveGame->bHasSeenFirstCutScene = true;
		UGameplayStatics::SaveGameToSlot(SaveGame, SaveGame->SaveSlotName, SaveGame->UserIndex);
	}
}
