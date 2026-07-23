#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTDecorator_Base.h"
#include "TPP_BTDecorator_CheckIfItemPossessed.generated.h"

UCLASS()
class TPP_01_API UTPP_BTDecorator_CheckIfItemPossessed : public UTPP_BTDecorator_Base
{
	GENERATED_BODY()

public:
	UTPP_BTDecorator_CheckIfItemPossessed();
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ItemKey;
};
