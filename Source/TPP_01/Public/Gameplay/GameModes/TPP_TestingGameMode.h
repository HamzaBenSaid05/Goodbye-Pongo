#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPP_TestingGameMode.generated.h"

UCLASS()
class TPP_01_API ATPP_TestingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE class ATPP_PlayerCharacter* GetPlayerCharacterRef() const { return PlayerRef.Get(); }

	UFUNCTION(BlueprintPure)
	FORCEINLINE class ATPP_Pongo* GetPongoRef() const { return PongoRef.Get(); }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TWeakObjectPtr<class APlayerController> PlayerControllerRef;
	UPROPERTY()
	TWeakObjectPtr<class ATPP_PlayerCharacter> PlayerRef;
	UPROPERTY()
	TWeakObjectPtr<class ATPP_Pongo> PongoRef;
};
