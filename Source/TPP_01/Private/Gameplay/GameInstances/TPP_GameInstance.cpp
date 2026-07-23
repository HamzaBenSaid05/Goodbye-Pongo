#include "Gameplay/GameInstances/TPP_GameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "SaveSystem/TPP_SaveGame.h"
#include "SaveSystem/TPP_SaveGameSetting.h"

void UTPP_GameInstance::ResetSaveGame()
{
	CurrentRoomIndex = INDEX_NONE;

	if (!IsValid(SaveGameObject))
		return;

	const FString SaveSlotName = SaveGameObject->SaveSlotName;
	const int32 UserIndex = SaveGameObject->UserIndex;
	
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SaveSlotName, UserIndex);
	}

	// Create a new SaveGameObject after deleting the existing one
	SaveGameObject = Cast<UTPP_SaveGame>(
		UGameplayStatics::CreateSaveGameObject(UTPP_SaveGame::StaticClass())
	);
}

int32 UTPP_GameInstance::GetCurrentGraphicsSettings() const
{

	if (SaveGameSetting) {
		return SaveGameSetting->CurrentGraphicsSettings;
	}

	return CurrentGraphicsSettings;
}

int32 UTPP_GameInstance::SetCurrentGraphicsSettings(const int32 InGraphicsSettings)
{

	if (SaveGameSetting) {
		SaveGameSetting->CurrentGraphicsSettings = InGraphicsSettings;
		return SaveGameSetting->CurrentGraphicsSettings;
	}

	CurrentGraphicsSettings = InGraphicsSettings;
	return CurrentGraphicsSettings;
}

void UTPP_GameInstance::SetDeactivateTutorial(const bool bInDeactivateTutorial)
{
	if(SaveGameSetting)
	{
		SaveGameSetting->bDeactivateTutorial = bInDeactivateTutorial;
	}

	bDeactivateTutorial = bInDeactivateTutorial;
}

bool UTPP_GameInstance::GetDeactivateTutorial() const
{
	if(SaveGameSetting)
	{
		return SaveGameSetting->bDeactivateTutorial;
	}
	return bDeactivateTutorial;
}

void UTPP_GameInstance::SetDeactivateHud(const bool bInDeactivateHud)
{
	if(SaveGameSetting)
	{
		SaveGameSetting->bDeactivateHud = bInDeactivateHud;
	}
	bDeactivateHud = bInDeactivateHud;
}

bool UTPP_GameInstance::GetDeactivateHud() const
{
	if(SaveGameSetting)
	{
		return SaveGameSetting->bDeactivateHud;
	}

	return bDeactivateHud;
}

// void UTPP_GameInstance::TriggerSave()
// {
// 	UGameplayStatics::SaveGameToSlot(SaveGameObject, SaveGameObject->SaveSlotName, SaveGameObject->UserIndex);
// }
//
// void UTPP_GameInstance::TriggerSave(UTPP_SaveGame* SaveGameToUse)
// {
// 	SaveGameObject = SaveGameToUse;
//
// 	TriggerSave();
// }

void UTPP_GameInstance::Init()
{
	Super::Init();

	if
	(
		UTPP_SaveGame* SaveGameInstance = Cast<UTPP_SaveGame>(
			UGameplayStatics::LoadGameFromSlot(
				GetDefault<UTPP_SaveGame>()->SaveSlotName,
				GetDefault<UTPP_SaveGame>()->UserIndex)
		)
	)
	{
		SaveGameObject = SaveGameInstance;
		CurrentRoomIndex = SaveGameObject->RoomIndex;
	}
	else
	{
		SaveGameObject = Cast<UTPP_SaveGame>(UGameplayStatics::CreateSaveGameObject(UTPP_SaveGame::StaticClass()));
	}

	if
(
	UTPP_SaveGameSetting* SaveGameSettingInstance = Cast<UTPP_SaveGameSetting>(
		UGameplayStatics::LoadGameFromSlot(
			GetDefault<UTPP_SaveGameSetting>()->SaveSlotName,
			GetDefault<UTPP_SaveGameSetting>()->UserIndex)
	)
)
{
	SaveGameSetting = SaveGameSettingInstance;
}
	else
	{
		SaveGameSetting = Cast<UTPP_SaveGameSetting>(UGameplayStatics::CreateSaveGameObject(UTPP_SaveGameSetting::StaticClass()));
	}

}

void UTPP_GameInstance::Shutdown()
{
	Super::Shutdown();

	SaveGameObject->RoomIndex = CurrentRoomIndex;

	UGameplayStatics::SaveGameToSlot(SaveGameObject, SaveGameObject->SaveSlotName, SaveGameObject->UserIndex);
}
