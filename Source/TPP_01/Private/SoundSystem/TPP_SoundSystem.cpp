#include "SoundSystem/TPP_SoundSystem.h"

#include "Components/AudioComponent.h"
#include "DeveloperSettings/TPP_SoundDeleveloperSettings.h"
#include "Sound/SoundSubmix.h"
#include "EventProxies/TPP_AudioProxySubsystem.h"
#include "GameFramework/GameMode.h"
#include "Gameplay/GameInstances/TPP_GameInstance.h"
#include "Gameplay/GameModes/TPP_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "SoundSystem/Data/TPP_AudioRow.h"

void UTPP_SoundSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UTPP_AudioProxySubsystem* Proxy = GetWorld()->GetSubsystem<UTPP_AudioProxySubsystem>())
	{
		Proxy->OnAudioTableEvent.AddDynamic(this, &UTPP_SoundSystem::PlaySound);
	}

	SoundSettings = GetDefault<UTPP_SoundDeleveloperSettings>();

	//FWorldDelegates::OnPostWorldInitialization.AddUObject(
	//this,
	//&ThisClass::HandleWorldLoaded);

	if (GEngine)
	{
		for (const FWorldContext& context : GEngine->GetWorldContexts())
		{
			UWorld* world = context.World();
			if (
				world &&
				world->IsGameWorld()
			)
			{
				HandleWorldLoaded(world);
				break; // Usually, only one game worldis active at a time
			}
		}
	}
}

void UTPP_SoundSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	if (UTPP_GameInstance* GI = InWorld.GetGameInstance<UTPP_GameInstance>())
	{
		if (UTPP_SaveGame* SaveGame = GI->GetSaveGameObject())
		{
			SetMasterVolume(SaveGame->VolumeMaster);
			SetBGMVolume(SaveGame->VolumeMusic);
			SetSFXVolume(SaveGame->VolumeSFX);
			SetAmbienceVolume(SaveGame->VolumeEnv);
		}
	}
}

void UTPP_SoundSystem::Deinitialize()
{
	SoundSettings = nullptr;
	CurrentWorld = nullptr;

	Super::Deinitialize();
}

void UTPP_SoundSystem::PlayBlueprintSound(FTPP_AudioRow & Row, class AActor * SourceActor, class UAudioComponent*  AudioComp, FVector Location)
{
	USceneComponent* AttachComponent = SourceActor->GetRootComponent();

	if (Row.SpatializationType == TPP_AudioSpatialization::ThreeDs)
	{
		if (Row.FollowModeType == TPP_AudioTargetFollowMode::TargetLocation)
		{
			AudioComp = UGameplayStatics::SpawnSoundAttached(Row.Sound, AttachComponent);
		}
		else { AudioComp = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Row.Sound, Location); }
	}
	else { AudioComp = UGameplayStatics::SpawnSound2D(GetWorld(), Row.Sound); }
}

void UTPP_SoundSystem::PlaySound(FTPP_AudioRow& Row, class UAudioComponent*& AudioComp, FVector& Location, FTPP_AudioParameter& Parameter, class USceneComponent* SourceActor)
{
	if (Row.SpatializationType == TPP_AudioSpatialization::ThreeDs)
	{
		if (Row.FollowModeType == TPP_AudioTargetFollowMode::TargetLocation)
		{
			AudioComp = UGameplayStatics::SpawnSoundAttached(Row.Sound, SourceActor);
		}
		else { AudioComp = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Row.Sound, Location); }
	}
	else { AudioComp = UGameplayStatics::SpawnSound2D(GetWorld(), Row.Sound); }
	if (AudioComp == nullptr) return;
	AudioComp->SetIntParameter(Parameter.Name, Parameter.Int);
	AudioComp->SetUISound(Row.bIsUISound);
}

void UTPP_SoundSystem::SetMasterVolume(float NewMasterVolume)
{
	SetLevel(
		SoundSettings->MasterMixer,
		Levels.master,
		NewMasterVolume
	);
}

void UTPP_SoundSystem::SetAmbienceVolume(float NewAmbienceVolume)
{
	SetLevel(
		SoundSettings->ambienceMixer,
		Levels.ambience,
		NewAmbienceVolume
	);
}

void UTPP_SoundSystem::SetBGMVolume(float NewBgmVolume)
{
	SetLevel(
		SoundSettings->musicMixer,
		Levels.music,
		NewBgmVolume
	);
}

void UTPP_SoundSystem::SetSFXVolume(float NewSFXVolume)
{
	SetLevel(
		SoundSettings->effectsMixer,
		Levels.effects,
		NewSFXVolume
	);
}

void UTPP_SoundSystem::SetGUIVolume(float NewGUIVolume)
{
	SetLevel(
		SoundSettings->userInterfaceMixer,
		Levels.userInterface,
		NewGUIVolume
	);
}

void UTPP_SoundSystem::SetVoiceVolume(float NewVoiceVolume)
{
	SetLevel(
		SoundSettings->voiceMixer,
		Levels.voice,
		NewVoiceVolume
	);
}

void UTPP_SoundSystem::SetLevels(const FTPP_SoundLevel& NewLevels)
{
	SetMasterVolume(NewLevels.master);
	SetAmbienceVolume(NewLevels.ambience);
	SetBGMVolume(NewLevels.music);
	SetSFXVolume(NewLevels.effects);
	SetGUIVolume(NewLevels.userInterface);
	SetVoiceVolume(NewLevels.voice);
}

void UTPP_SoundSystem::SetLevel(const TSoftObjectPtr<class USoundSubmix>& Submix, float& Level, float NewLevel)
{
	Level = FMath::Clamp(NewLevel, 0.0f, 1.0f);

	if (!CurrentWorld)
		return;

	USoundSubmix* LoadedSubmix = Submix.LoadSynchronous();

	if (!LoadedSubmix)
		return;

	float SafeValue = FMath::Max(Level, 0.0001f);
	LoadedSubmix->SetSubmixOutputVolume(CurrentWorld,SafeValue);
		GEngine->AddOnScreenDebugMessage(
			5,
			0.3f,
			FColor::Cyan,
			FString::SanitizeFloat(SafeValue)
		);

	OnLevelChanged.Broadcast(Submix, Level);
}

void UTPP_SoundSystem::HandleWorldLoaded(UWorld* NewWorld)
{
	//	Skip non-game world to avoid storing preview or editor worlds
	if (
		!NewWorld ||
		!NewWorld->IsGameWorld()
	)
		return;

	//	Store currently loaded world
	CurrentWorld = NewWorld;

	//	Re-apply audio levels as they were stored before
	SetLevels(Levels);
}
