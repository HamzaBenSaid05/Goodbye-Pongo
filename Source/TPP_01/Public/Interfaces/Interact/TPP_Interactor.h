#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TPP_Interactor.generated.h"

UINTERFACE(MinimalAPI)
class UTPP_Interactor : public UInterface
{
	GENERATED_BODY()
};

class TPP_01_API ITPP_Interactor
{
	GENERATED_BODY()

public:

	virtual void Interact(TObjectPtr<class UTPP_InteractAction_DA> Data) = 0;
	virtual void EndInteract(TObjectPtr<class UTPP_InteractAction_DA> Data) = 0;
	virtual bool IsInteracting() = 0;
};
