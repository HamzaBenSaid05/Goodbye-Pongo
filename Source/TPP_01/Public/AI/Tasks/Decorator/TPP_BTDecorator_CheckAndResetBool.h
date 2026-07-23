#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTDecorator_Base.h"
#include "TPP_BTDecorator_CheckAndResetBool.generated.h"

UCLASS()
class TPP_01_API UTPP_BTDecorator_CheckAndResetBool : public UTPP_BTDecorator_Base
{
	GENERATED_BODY()

public:
	UTPP_BTDecorator_CheckAndResetBool();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult) override;
};
