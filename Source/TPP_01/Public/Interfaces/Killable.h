#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Enums/TPP_DeathInfo.h"
#include "Killable.generated.h"

UINTERFACE(Blueprintable)
class UKillable : public UInterface
{
	GENERATED_BODY()
};

class TPP_01_API IKillable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Kill(bool bSoftReset = false, const ECharacterDeath CharacterDeath = ECharacterDeath::PlayerDeath, const EDeathCause Cause = EDeathCause::KillBox);
};
