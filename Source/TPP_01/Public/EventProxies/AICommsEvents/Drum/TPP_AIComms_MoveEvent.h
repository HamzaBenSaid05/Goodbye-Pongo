#pragma once

#include "CoreMinimal.h"
#include "EventProxies/AICommsEvents/TPP_AICommsEventBase.h"
#include "TPP_AIComms_MoveEvent.generated.h"

UCLASS()
class TPP_01_API UTPP_AIComms_MoveEvent : public UTPP_AICommsEventBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FVector TargetLocation;
	UPROPERTY()
	AActor* TargetObject;
	virtual void Init(const FVector& Target, AActor* Object = nullptr)
	{
		this->TargetLocation = Target;
		if (Object != nullptr)
			this->TargetObject = Object;
	}
};
