#pragma once

#include "CoreMinimal.h"
#include "AI/Enum/TPP_PongoEnum.h"
#include "EventProxies/AICommsEvents/TPP_AICommsEventBase.h"
#include "TPP_AIUnlockStrollEvent.generated.h"

UCLASS()
class TPP_01_API UTPP_AIUnlockStrollEvent : public UTPP_AICommsEventBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bCanStroll;
	UPROPERTY()
	ETPP_PongoState StrollState;
	
	void Init()
	{
		bCanStroll = true;
		StrollState = ETPP_PongoState::Stroll;	
	}
};
