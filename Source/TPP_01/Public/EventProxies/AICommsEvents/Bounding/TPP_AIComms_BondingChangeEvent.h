#pragma once

#include "CoreMinimal.h"
#include "EventProxies/AICommsEvents/TPP_AICommsEventBase.h"
#include "TPP_AIComms_BondingChangeEvent.generated.h"

UCLASS()
class TPP_01_API UTPP_AIComms_BondingChangeEvent : public UTPP_AICommsEventBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float BondingValue;

	void Init(const float& NewBondingValue)
	{
		BondingValue = NewBondingValue;
	}
};
