#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NeedPowerSwitch.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UNeedPowerSwitch : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TPP_01_API INeedPowerSwitch
{
	GENERATED_BODY()

	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SwitchPower();
};
