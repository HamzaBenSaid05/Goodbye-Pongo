// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TPP_SaveGameSetting.generated.h"

/**
 *
 */
UCLASS()
class TPP_01_API UTPP_SaveGameSetting : public USaveGame
{
	GENERATED_BODY()

public:
	UTPP_SaveGameSetting();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Basic)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Basic)
	int32 UserIndex;
	
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
