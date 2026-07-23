#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TPP_AIPettable.generated.h"

UINTERFACE(Blueprintable)
class UTPP_AIPettable : public UInterface
{
	GENERATED_BODY()
};

class TPP_01_API ITPP_AIPettable
{
	GENERATED_BODY()

public:
	// Called to verify if a agent can be pet
	virtual bool CanBePet() = 0;

	// Called when pet on agent is finished
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PetFinished();
	
	// Called when pet on agent is started
	virtual void PetStart() = 0;
	
	// Called when pet on agent is unlocked
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UnlockPet();
};
