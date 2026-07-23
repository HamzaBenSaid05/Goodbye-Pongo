#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Resettable.generated.h"

UINTERFACE()
class UResettable : public UInterface
{
	GENERATED_BODY()
};

class TPP_01_API IResettable
{
	GENERATED_BODY()

public:
	virtual void HandleReset() = 0;
};
