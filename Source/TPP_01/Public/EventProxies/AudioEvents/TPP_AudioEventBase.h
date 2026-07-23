#pragma once

#include "CoreMinimal.h"
#include "EventProxies/TPP_AudioProxySubsystem.h"
#include "UObject/Object.h"
#include "TPP_AudioEventBase.generated.h"

UCLASS()
class TPP_01_API UTPP_AudioEventBase : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	FVector Location;
	UPROPERTY(BlueprintReadOnly)
	float Volume;
};
