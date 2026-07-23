#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TPP_AIEmotionalStateObject.generated.h"

UINTERFACE()
class UTPP_AIEmotionalStateObject : public UInterface
{
	GENERATED_BODY()
};

class TPP_01_API ITPP_AIEmotionalStateObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AI|EmotionalStateObject")
	void SetEmotionalStateObject(AActor* NewEmotionalStateObject);
	virtual AActor* GetEmotionalStateObject() const = 0;
};
