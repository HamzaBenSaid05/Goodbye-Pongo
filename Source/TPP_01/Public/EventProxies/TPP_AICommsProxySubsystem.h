#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TPP_AICommsProxySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAICommEventDelegate, const class UTPP_AICommsEventBase*, AICommsEvent);

UCLASS()
class TPP_01_API UTPP_AICommsProxySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FAICommEventDelegate OnAICommEventTriggered;

	UFUNCTION(BlueprintCallable)
	void NotifyAICommEvent(const class UTPP_AICommsEventBase* AICommEvent);
};
