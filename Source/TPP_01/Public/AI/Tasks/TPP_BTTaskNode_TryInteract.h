#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTTaskNode_Base.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TPP_BTTaskNode_TryInteract.generated.h"

UCLASS()
class TPP_01_API UTPP_BTTaskNode_TryInteract : public UTPP_BTTaskNode_Base
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector EmotionalStateObjectKey;
};
