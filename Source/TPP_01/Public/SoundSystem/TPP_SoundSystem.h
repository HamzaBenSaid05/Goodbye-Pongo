// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/TPP_AudioRow.h"
#include "Data/TPP_SoundDelegates.h"
#include "Data/TPP_SoundLevel.h"
#include "DeveloperSettings/TPP_SoundDeleveloperSettings.h"
#include "Subsystems/WorldSubsystem.h"
#include "TPP_SoundSystem.generated.h"

UCLASS()
class TPP_01_API UTPP_SoundSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	UFUNCTION()
	void PlayBlueprintSound(FTPP_AudioRow& Row, class AActor* SourceActor, class UAudioComponent* AudioComp, FVector Location);
	UFUNCTION()
	void PlaySound(FTPP_AudioRow& Row, class UAudioComponent*& AudioComp, FVector& Location,FTPP_AudioParameter& Parameter, class USceneComponent* SourceActor);
	// Fields
	UPROPERTY(BlueprintAssignable, Category = "Audio Events")
	FSoundLevelDelegate OnLevelChanged;

	FORCEINLINE const class UTPP_SoundDeleveloperSettings* GetSettings() const { return SoundSettings; }

	UFUNCTION(BlueprintPure, Category = "Audio")
	FORCEINLINE float GetMasterVolume() const { return Levels.master; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetMasterVolume(float NewMasterVolume);
	UFUNCTION(BlueprintPure, Category = "Audio")
	FORCEINLINE float GetAmbienceVolume() const { return Levels.ambience; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetAmbienceVolume(float NewAmbienceVolume);
	UFUNCTION(BlueprintPure, Category = "Audio")
	FORCEINLINE float GetBGMVolume() const { return Levels.music; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetBGMVolume(float NewBgmVolume);
	UFUNCTION(BlueprintPure, Category = "Audio")
	FORCEINLINE float GetSFXVolume() const { return Levels.effects; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetSFXVolume(float NewSFXVolume);
	UFUNCTION(BlueprintPure, Category = "Audio")
	FORCEINLINE float GetGUIVolume() const { return Levels.userInterface; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetGUIVolume(float NewGUIVolume);
	UFUNCTION(BlueprintPure, Category = "Audio")
	FORCEINLINE float GetVoiceVolume() const { return Levels.voice; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetVoiceVolume(float NewVoiceVolume);

	const FTPP_SoundLevel& GetLevels() const { return Levels; }
	UFUNCTION(BlueprintCallable, Category = "Audio", DisplayName = "Get Levels")
	FTPP_SoundLevel GetLevelsCopy() const { return Levels; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetLevels(const FTPP_SoundLevel& NewLevels);

private:
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetLevel(const TSoftObjectPtr<class USoundSubmix>& Submix, float& Level, float NewLevel);
	UFUNCTION()
	void HandleWorldLoaded(UWorld* NewWorld);
	UPROPERTY()
	TObjectPtr<UWorld> CurrentWorld;
	UPROPERTY()
	TObjectPtr<const class UTPP_SoundDeleveloperSettings> SoundSettings;
	UPROPERTY()
	FTPP_SoundLevel Levels;
};
