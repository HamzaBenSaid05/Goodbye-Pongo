#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TPP_PongoPlayerStart.generated.h"

UINTERFACE()
class UTPP_PongoPlayerStart : public UInterface
{
	GENERATED_BODY()
};

class TPP_01_API ITPP_PongoPlayerStart
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AI|Escape")
	bool GetShouldEscapeFromPlayer();
};
