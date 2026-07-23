#pragma once

#include "CoreMinimal.h"
#include "EventProxies/AICommsEvents/TPP_AICommsEventBase.h"
#include "TPP_AIComms_OnPetEvent.generated.h"

UCLASS()
class TPP_01_API UTPP_AIComms_OnPetEvent : public UTPP_AICommsEventBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float PetPercentValue;

	virtual void Init(const float& NewPetPercentValue)
	{
		this->PetPercentValue = NewPetPercentValue;
	}
};
