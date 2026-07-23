#pragma once

#include "CoreMinimal.h"
#include "EventProxies/AICommsEvents/TPP_AICommsEventBase.h"
#include "TPP_AIComms_ChangeEscapeFromPlayerDistance.generated.h"

UCLASS()
class TPP_01_API UTPP_AIComms_ChangeEscapeFromPlayerDistance : public UTPP_AICommsEventBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float EscapeFromPlayerDistanceMax;
	UPROPERTY()
	float EscapeFromPlayerDistanceMin;
	
	virtual void Init(const float& NewEscapeFromPlayerDistanceMax, const float& NewEscapeFromPlayerDistanceMin)
	{
		this->EscapeFromPlayerDistanceMax = NewEscapeFromPlayerDistanceMax;
		this->EscapeFromPlayerDistanceMin = NewEscapeFromPlayerDistanceMin;
	}
};
