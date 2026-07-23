#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SoundSystem/Data/TPP_SoundLevel.h"
#include "TPP_SoundDeleveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config = GameSound, DefaultConfig)
class TPP_01_API UTPP_SoundDeleveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UTPP_SoundDeleveloperSettings();
	//	Supposed to receive all sound routed from other submixes
	UPROPERTY(Config, EditAnywhere, Category = "Sound Classes")
	TSoftObjectPtr<class USoundSubmix> MasterMixer;
	//	Supposed to receive sound from ambient sources
	UPROPERTY(Config, EditAnywhere, Category = "Sound Classes")
	TSoftObjectPtr<class USoundSubmix> ambienceMixer;
	//	Supposed to receive sound from music sources
	UPROPERTY(Config, EditAnywhere, Category = "Sound Classes")
	TSoftObjectPtr<class USoundSubmix> musicMixer;
	//	Supposed to receive sound from effects (explosions, footsteps...)
	UPROPERTY(Config, EditAnywhere, Category = "Sound Classes")
	TSoftObjectPtr<class USoundSubmix> effectsMixer;
	//	Supposed to receive sound from user interfaces
	UPROPERTY(Config, EditAnywhere, Category = "Sound Classes")
	TSoftObjectPtr<class USoundSubmix> userInterfaceMixer;
	//	Supposed to receive sound from speech (cinematics, context...)
	UPROPERTY(Config, EditAnywhere, Category = "Sound Classes")
	TSoftObjectPtr<class USoundSubmix> voiceMixer;

	/*
	 * Default levels get applied as the audio subsystem is
	 * created and are likely overridden as soon as any custom
	 * setting is loaded.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Levels")
	FTPP_SoundLevel defaultLevels;
};
