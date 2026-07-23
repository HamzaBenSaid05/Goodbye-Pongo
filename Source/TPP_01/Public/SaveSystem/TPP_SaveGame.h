#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AI/Struct/PongoStructs.h"
#include "GameFramework/SaveGame.h"
#include "TPP_SaveGame.generated.h"

UCLASS()
class TPP_01_API UTPP_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UTPP_SaveGame();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Basic)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Basic)
	int32 UserIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Room)
	int32 RoomIndex = INDEX_NONE;

	// Pongo Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Pongo)
	float BondingValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Pongo)
	ETPP_PongoState BondingState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Pongo)
	bool bCanStroll;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Pongo)
	bool bShouldFearFromPlayer = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Pongo)
	bool bCanBePet = false;
	// Pongo Variables

	UPROPERTY(VisibleAnywhere,  BlueprintReadWrite, Category = Drum)
	bool bInteractActive;

	UPROPERTY(VisibleAnywhere,  BlueprintReadWrite, Category = Drum)
	bool bGoThereActive;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Drum)
	bool bHasDrum;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Doors)
	FGameplayTagContainer UsedDoors;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CutScene)
	bool bHasSeenFirstCutScene = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CutScene)
	bool bHasSeenPetCutScene = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CutScene)
	bool bHasSeenRuruFruitCutscene = false;
	
	UPROPERTY()
	bool bDeactivateTutorial;

	UPROPERTY()
	bool bDeactivateHud;

	UPROPERTY()
	int32 CurrentGraphicsSettings = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Audio)
	float VolumeMaster = 100;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Audio)
	float VolumeMusic = 100;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Audio)
	float VolumeEnv = 100;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Audio)
	float VolumeSFX = 100;
	
};
