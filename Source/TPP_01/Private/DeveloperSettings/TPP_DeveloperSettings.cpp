#include "DeveloperSettings/TPP_DeveloperSettings.h"

UTPP_DeveloperSettings::UTPP_DeveloperSettings()
{
	CategoryName = "Gameplay";
}

bool UTPP_DeveloperSettings::IsGameplayLevel(const UWorld* Level) const
{
	const FName LevelPackageName =
		FName(
		      *UWorld::RemovePIEPrefix(Level->PersistentLevel->GetPackage()->GetName())
		     );

	//	Determine whether the level instance is listed in the settings list
	bool IsLevelInList = false;
	for (const TSoftObjectPtr<UWorld>& Gameplaylevel : GameplayLevelsList)
		if (Gameplaylevel.GetLongPackageFName() == LevelPackageName) //	Using FName comparison, more performant
		{
			IsLevelInList = true;
			break;
		}

	//	Return whether the level is considered a gameplay level or not
	return IsLevelInList ^ bGameplayLevelsListIsWhitelist;
}
