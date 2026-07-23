#pragma once

#include "CoreMinimal.h"
#include "AI/Tasks/Decorator/Base/TPP_BTDecorator_Base.h"
#include "TPP_BTDecorator_HasItem.generated.h"

UCLASS()
class TPP_01_API UTPP_BTDecorator_HasItem : public UTPP_BTDecorator_Base
{
	GENERATED_BODY()
	
public:
	UTPP_BTDecorator_HasItem();

private:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
