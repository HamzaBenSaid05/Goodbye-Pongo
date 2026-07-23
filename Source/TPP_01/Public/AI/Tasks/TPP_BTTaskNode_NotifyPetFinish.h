#pragma once

#include "CoreMinimal.h"
#include "AI/Tasks/Base/TPP_BTTaskNode_Base.h"
#include "TPP_BTTaskNode_NotifyPetFinish.generated.h"

UCLASS()
class TPP_01_API UTPP_BTTaskNode_NotifyPetFinish : public UTPP_BTTaskNode_Base
{
	GENERATED_BODY()

public:
	UTPP_BTTaskNode_NotifyPetFinish();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
