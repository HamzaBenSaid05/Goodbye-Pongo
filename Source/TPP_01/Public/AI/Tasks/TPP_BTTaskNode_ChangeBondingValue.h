#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTTaskNode_Base.h"
#include "TPP_BTTaskNode_ChangeBondingValue.generated.h"

UCLASS()
class TPP_01_API UTPP_BTTaskNode_ChangeBondingValue : public UTPP_BTTaskNode_Base
{
	GENERATED_BODY()

public:
	UTPP_BTTaskNode_ChangeBondingValue();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UPROPERTY(EditAnywhere, Category="AI")
	float BondingValueChange;
};
