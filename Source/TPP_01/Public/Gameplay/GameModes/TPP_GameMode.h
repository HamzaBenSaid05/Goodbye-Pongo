#pragma once

#include "CoreMinimal.h"
#include "EventProxies/RoomsEvents/TPP_RoomEventBase.h"
#include "GameFramework/GameModeBase.h"
#include "Enums/TPP_DeathInfo.h"
#include "TPP_GameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameOver,ECharacterDeath,CharacterDeath,EDeathCause,DeathCause);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerReady, AActor*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivateLoadingScreen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeactivateLoadingScreen);


UCLASS()
class TPP_01_API ATPP_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATPP_GameMode();

	UPROPERTY()
	FOnActivateLoadingScreen OnActivateLoadingScreen;
	UPROPERTY()
	FOnDeactivateLoadingScreen OnDeactivateLoadingScreen;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerReady OnPlayerReady;
	UPROPERTY(BlueprintAssignable)
	FOnGameOver OnGameOver;

	UFUNCTION(BlueprintPure)
	FORCEINLINE class ATPP_PlayerCharacter* GetPlayerCharacterRef() const { return PlayerRef.Get(); }

	UFUNCTION(BlueprintPure)
	FORCEINLINE class ATPP_Pongo* GetPongoRef() const { return PongoRef.Get(); }

	UFUNCTION(BlueprintPure)
	FORCEINLINE class ATPP_LevelSequencePlayer* GetLevelSequencePlayer() const { return LevelSequencePlayer.Get(); }

	UFUNCTION(BlueprintPure)
	void GetCharacters(TArray<AActor*>& InArray) const;
	
	UFUNCTION(BlueprintCallable)
	void Restart();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CutScene")
	TWeakObjectPtr<class ATPP_LevelSequencePlayer> LevelSequencePlayer;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TSubclassOf<ACharacter> CharacterClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TSoftObjectPtr<UWorld> MainLevel;

	UPROPERTY(EditDefaultsOnly, meta=(UIMin = -1, ClampMin = -1, Category="Room Management",
		Tooltip="The index of the room pongo will start in, used to decide whether to spawn pongo alongside the player"))
	int32 PongoStartRoomIndex = INDEX_NONE;
	
	UPROPERTY(EditDefaultsOnly, Category="Room Management", meta=(
		Tooltip="This list should contain all the rooms indexes we want to consider for spawn when Jane has entered them"))
	TArray<int32> JaneSpawnRoomsAllowList;

	UPROPERTY(EditDefaultsOnly, Category="Item Highlighting")
	float ItemScanningInterval;
	UPROPERTY(EditDefaultsOnly, Category="Item Highlighting", meta=(ToolTip="This will get squared", UIMin=0, ClampMin=0))
	float MaxDistance;

	UPROPERTY()
	TWeakObjectPtr<class UTPP_RoomStreamingSubsystem> RoomStreamingSubsystem;

	UPROPERTY()
	TWeakObjectPtr<class APlayerController> PlayerControllerRef;
	UPROPERTY()
	TWeakObjectPtr<class ATPP_PlayerCharacter> PlayerRef;
	UPROPERTY()
	TWeakObjectPtr<class ATPP_Pongo> PongoRef;
	UPROPERTY()
	TWeakObjectPtr<class  UTPP_GameInstance> GI;
	
	UPROPERTY(EditDefaultsOnly, Category="CutScene|LevelSequence")
	TSoftObjectPtr<class ULevelSequence> LevelSequenceToPlayOnStartSoft;

	UFUNCTION()
	void HandlePlayerChangedRoom(AActor* NewCamera, float InBlendTime);

	UFUNCTION()
	void HandleNewRoomsLoaded();

	UFUNCTION()
	void SpawnPlayer(AActor* PlayerStartToUse, AActor* PongoPlayerStart);

	UFUNCTION()
	void PongoChangedRoom(const class UTPP_RoomEventBase* RoomEvent);
	
	UFUNCTION()
	void HandleGameOver(FDeathInfo DeathInfo);

	bool bShouldSoftReset;
	
	FVector StartingOPongoLocation;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category="Room Management",
		meta=(UIMin = -1, ClampMin = -1, Tooltip="Used only in editor, set this to force spawn in a certain room"))
	int32 SpawnRoomIndex = INDEX_NONE;
#endif

	UFUNCTION()
	void OnHUDReady();
};
