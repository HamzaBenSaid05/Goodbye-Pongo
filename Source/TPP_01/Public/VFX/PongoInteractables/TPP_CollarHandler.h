#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TPP_CollarHandler.generated.h"

UINTERFACE(Blueprintable)
class UTPP_CollarHandler : public UInterface
{
	GENERATED_BODY()
};

class TPP_01_API ITPP_CollarHandler
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TogglePulse(bool bTurnOn);
};
