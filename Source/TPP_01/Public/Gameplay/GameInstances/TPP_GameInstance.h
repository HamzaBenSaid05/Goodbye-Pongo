#pragma once

#include "CoreMinimal.h"
#include "ShaderPipelineCache.h"
#include "Engine/GameInstance.h"
#include "SaveSystem/TPP_SaveGame.h"
#include "TPP_GameInstance.generated.h"

UCLASS()
class TPP_01_API UTPP_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCurrentRoomIndex(const int InIndex) { CurrentRoomIndex = InIndex; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE int GetRoomIndexToLoad() const { return CurrentRoomIndex; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UTPP_SaveGame* GetSaveGameObject() const { return SaveGameObject; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UTPP_SaveGameSetting* GetSaveGameSetting() const { return SaveGameSetting; }
	
	UFUNCTION(BlueprintPure)
	int32 GetShaderCachesRemaining() { return FShaderPipelineCache::NumPrecompilesRemaining(); }

	UFUNCTION(BlueprintCallable)
	void ResetSaveGame();

	int32 GetCurrentGraphicsSettings() const;

	int32 SetCurrentGraphicsSettings(const int32 InGraphicsSettings);

	UFUNCTION(BlueprintCallable)
	void SetDeactivateTutorial(const bool bInDeactivateTutorial);

	UFUNCTION(BlueprintCallable)
	bool GetDeactivateTutorial() const;

	UFUNCTION(BlueprintCallable)
	void SetDeactivateHud(const bool bInDeactivateHud);
	UFUNCTION(BlueprintCallable)
	bool GetDeactivateHud() const;
	

	FORCEINLINE bool GetHasGameBeenOpened() const { return bHasGameBeenOpened; }
	FORCEINLINE void SetHasGameBeenOpened(const bool bInHasGameBeenOpened) { bHasGameBeenOpened = bInHasGameBeenOpened; }
	// void TriggerSave();
	// void TriggerSave(UTPP_SaveGame* SaveGameToUse);

protected:
	virtual void Init() override;
	virtual void Shutdown() override;
	UPROPERTY(EditAnywhere,Category = "Graphics Settings")
	int32 CurrentGraphicsSettings = 2;

	UPROPERTY(EditAnywhere)
	bool bDeactivateTutorial;
	UPROPERTY(EditAnywhere)
	bool bDeactivateHud;
private:
	UPROPERTY()
	int32 CurrentRoomIndex = INDEX_NONE;

	UPROPERTY()
	class UTPP_SaveGame* SaveGameObject;

	UPROPERTY()
	class UTPP_SaveGameSetting* SaveGameSetting;

	UPROPERTY()
	bool bHasGameBeenOpened = false;
};
