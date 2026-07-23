#pragma once

#include "CoreMinimal.h"
#include "AI/Enum/TPP_PongoEnum.h"
#include "EventProxies/AICommsEvents/TPP_AICommsEventBase.h"
#include "TPP_AIComms_ChangeEmotionalStateEvent.generated.h"

UCLASS()
class TPP_01_API UTPP_AIComms_ChangeEmotionalStateEvent : public UTPP_AICommsEventBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ETPP_PongoState EmotionalState;
	UPROPERTY()
	AActor* EmotionalStateObject;

	void Init(ETPP_PongoState NewEmotionalState, AActor* Object = nullptr)
	{
		this->EmotionalState = NewEmotionalState;
		if (Object != nullptr)
			this->EmotionalStateObject = Object;
	}
};
