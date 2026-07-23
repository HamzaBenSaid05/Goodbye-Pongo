#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TPP_DeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Gameplay, DefaultConfig)
class TPP_01_API UTPP_DeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	UTPP_DeveloperSettings();

private:
	UPROPERTY(Config, EditAnywhere, Category = "Gameplay Levels")
	TSet<TSoftObjectPtr<UWorld>> GameplayLevelsList;
	UPROPERTY(Config, EditAnywhere, Category = "Gameplay Levels")
	bool bGameplayLevelsListIsWhitelist = true;

public:
	UFUNCTION(BlueprintPure, Category = "Gameplay Levels")
	bool IsGameplayLevel(const UWorld* Level) const;
};
