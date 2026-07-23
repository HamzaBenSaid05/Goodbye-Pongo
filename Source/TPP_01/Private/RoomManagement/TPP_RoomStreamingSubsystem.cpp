#include "RoomManagement/TPP_RoomStreamingSubsystem.h"

#include "DeveloperSettings/TPP_DeveloperSettings.h"
#include "GameFramework/PlayerStart.h"
#include "Gameplay/GameInstances/TPP_GameInstance.h"
#include "Kismet/GameplayStatics.h"

void UTPP_RoomStreamingSubsystem::InitializeFromSave(int32 SavedIndex)
{
	DiscoverRooms();

	if (!OrderedRooms.IsValidIndex(SavedIndex))
		SavedIndex = 0;

	CurrentRoomIndex = SavedIndex;
	PlayerCurrentRoomIndex = SavedIndex;

	ULevelStreaming* SpawnRoom = OrderedRooms[PlayerCurrentRoomIndex];

	SpawnRoom->OnLevelShown.AddDynamic(this, &UTPP_RoomStreamingSubsystem::OnInitialRoomShown);

	LoadLevel(SpawnRoom);
}

void UTPP_RoomStreamingSubsystem::NotifyRoomEntered(ULevelStreaming* RoomLevel)
{
	int32 Index = OrderedRooms.IndexOfByKey(RoomLevel);

	if (Index == INDEX_NONE)
		return;

	if (Index == PlayerCurrentRoomIndex)
		return;

	PlayerCurrentRoomIndex = Index;
	
	if (JaneSpawnRoomsAllowList.Contains(PlayerCurrentRoomIndex))
		UpdateGameInstanceRoomIndex(PlayerCurrentRoomIndex);

	UpdateLoadedRooms(PlayerCurrentRoomIndex);
}

void UTPP_RoomStreamingSubsystem::UpdateCompanionRoom(AActor* NewCompanionRoom)
{
	const int PongoLevelIndex = OrderedRooms.IndexOfByKey(ULevelStreaming::FindStreamingLevel(NewCompanionRoom->GetLevel()));
	CompanionCurrentRoomIndex = PongoLevelIndex != INDEX_NONE ? PongoLevelIndex : CompanionCurrentRoomIndex;

	if (!JaneSpawnRoomsAllowList.Contains(CompanionCurrentRoomIndex))
		UpdateGameInstanceRoomIndex(CompanionCurrentRoomIndex);
}

bool UTPP_RoomStreamingSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
		return false;

	const UTPP_DeveloperSettings* GameplaySettings = GetDefault<UTPP_DeveloperSettings>();
	check(GameplaySettings);

	const UWorld* Level = CastChecked<UWorld>(Outer);

	return GameplaySettings->IsGameplayLevel(Level);
}

void UTPP_RoomStreamingSubsystem::DiscoverRooms()
{
	OrderedRooms = GetWorld()->GetStreamingLevels();

	OrderedRooms.RemoveAll([](const ULevelStreaming* L) { return L == nullptr || L->IsLevelLoaded(); });

	// // Sort by X world location
	OrderedRooms.Sort
	([](const ULevelStreaming& A, const ULevelStreaming& B)
	{
		return A.LevelTransform.GetLocation().X <
		B.LevelTransform.GetLocation().X;
	});
}

void UTPP_RoomStreamingSubsystem::UpdateLoadedRooms(int32 CenterIndex)
{
	TSet<ULevelStreaming*> Desired;

	// Set StartIndex at the leftmost neighbor of either the companion or the player, whichever is further left,
	// and end index at the rightmost neighbor of the player
	const int32 StartIndex = FMath::Min(CompanionCurrentRoomIndex - NeighborDepth, CenterIndex - NeighborDepth);
	const int32 EndIndex = CenterIndex + NeighborDepth;
	for (int32 i = StartIndex; i <= EndIndex; ++i)
	{
		if (OrderedRooms.IsValidIndex(i))
		{
			Desired.Add(OrderedRooms[i]);
		}
	}

	// Load missing
	for (ULevelStreaming* Level : Desired) { if (!LoadedRooms.Contains(Level)) { LoadLevel(Level); } }

	// Unload far rooms
	for (ULevelStreaming* Level : LoadedRooms) { if (!Desired.Contains(Level)) { UnloadLevel(Level); } }

	LoadedRooms = Desired;
}

void UTPP_RoomStreamingSubsystem::LoadLevel(ULevelStreaming* Level)
{
	Level->SetShouldBeLoaded(true);
	Level->SetShouldBeVisible(true);
	if (!Level->OnLevelShown.IsBound())
		Level->OnLevelShown.AddDynamic(this, &UTPP_RoomStreamingSubsystem::OnNewRoomLoaded);
}

void UTPP_RoomStreamingSubsystem::UnloadLevel(ULevelStreaming* Level)
{
	Level->SetShouldBeLoaded(false);
	Level->SetShouldBeVisible(false);
	if (Level->OnLevelShown.IsBound())
		Level->OnLevelShown.RemoveDynamic(this, &UTPP_RoomStreamingSubsystem::OnNewRoomLoaded);
}

void UTPP_RoomStreamingSubsystem::OnInitialRoomShown()
{
	OrderedRooms[PlayerCurrentRoomIndex]->OnLevelShown.RemoveAll(this);

	TArray<AActor*> PlayerStartActors;
	AActor* PlayerStartToUse = nullptr;
	AActor* PongoStartToUse = nullptr;

	const bool bShouldBringPongo = PlayerCurrentRoomIndex >= PongoStartRoomIndex;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartActors);

	if (!PlayerStartActors.IsEmpty())
	{
		for (AActor* PlayerStartActor : PlayerStartActors)
		{
			if (ULevelStreaming::FindStreamingLevel(PlayerStartActor->GetLevel()) == OrderedRooms[PlayerCurrentRoomIndex])
			{
				if (bShouldBringPongo)
				{
					if (PlayerStartActor->ActorHasTag(FName("PongoPlayerStart")))
						PongoStartToUse = PlayerStartActor;
					else
						PlayerStartToUse = PlayerStartActor;
				}
				else
				{
					PlayerStartToUse = PlayerStartActor;
					break;
				}
			}
		}
	}

	OnSpawnRoomReady.Broadcast(PlayerStartToUse, PongoStartToUse);

	UpdateLoadedRooms(PlayerCurrentRoomIndex);
}

void UTPP_RoomStreamingSubsystem::OnNewRoomLoaded() { OnNewRoomReady.Broadcast(); }

void UTPP_RoomStreamingSubsystem::UpdateGameInstanceRoomIndex(const int32 IndexToUse)
{
	if (UTPP_GameInstance* GI = Cast<UTPP_GameInstance>(GetWorld()->GetGameInstance()))
	{
		CurrentRoomIndex = IndexToUse;
		GI->SetCurrentRoomIndex(IndexToUse);
	}
}
