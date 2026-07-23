#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTDecorator_Base.h"
#include "TPP_BTDecorator_IsCuriosityAutoDestroyable.generated.h"

UCLASS()
class TPP_01_API UTPP_BTDecorator_IsCuriosityAutoDestroyable : public UTPP_BTDecorator_Base
{
	GENERATED_BODY()

public:
	UTPP_BTDecorator_IsCuriosityAutoDestroyable();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
