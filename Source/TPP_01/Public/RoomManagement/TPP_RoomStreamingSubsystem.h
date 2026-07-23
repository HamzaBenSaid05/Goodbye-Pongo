#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TPP_RoomStreamingSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpawnRoomReadyDelegate, AActor*, PlayerStartToUse, AActor*, PongoPlayerStart);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNewRoomReadyDelegate);

UCLASS(Blueprintable)
class TPP_01_API UTPP_RoomStreamingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	FSpawnRoomReadyDelegate OnSpawnRoomReady;
	FNewRoomReadyDelegate OnNewRoomReady;

	// Initializes the subsystem based on the saved room index, loading the corresponding level and its neighbors
	void InitializeFromSave(int32 SavedIndex);
	// Notifies the subsystem that the player has entered a new room, triggering current levels update
	void NotifyRoomEntered(ULevelStreaming* RoomLevel);

	UFUNCTION(BlueprintPure)
	int GetCurrentRoomIndex() const { return CurrentRoomIndex; }

	UFUNCTION(BlueprintPure)
	const TArray<ULevelStreaming*>& GetRooms() const { return OrderedRooms; }

	UFUNCTION()
	void UpdateCompanionRoom(AActor* NewCompanionRoom);
	
	int32 PongoStartRoomIndex;
	
	UPROPERTY(Transient)
	TArray<int32> JaneSpawnRoomsAllowList;

private:
	UPROPERTY()
	TArray<ULevelStreaming*> OrderedRooms;
	UPROPERTY()
	TSet<ULevelStreaming*> LoadedRooms;

	int32 PlayerCurrentRoomIndex = INDEX_NONE;
	int32 CurrentRoomIndex = INDEX_NONE;
	int32 CompanionCurrentRoomIndex = 0;

	// How many neighbors to keep loaded
	UPROPERTY(EditDefaultsOnly)
	int32 NeighborDepth = 1; // 1 = current + left + right

	// Discovers all streaming levels in the world and orders them based on their X world location
	void DiscoverRooms();
	// Updates the set of loaded rooms based on the new center index, loading/unloading levels as needed
	void UpdateLoadedRooms(int32 CenterIndex);

	void LoadLevel(ULevelStreaming* Level);
	void UnloadLevel(ULevelStreaming* Level);

	UFUNCTION()
	void OnInitialRoomShown();

	UFUNCTION()
	void OnNewRoomLoaded();
	
	void UpdateGameInstanceRoomIndex(const int32 IndexToUse);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
};
