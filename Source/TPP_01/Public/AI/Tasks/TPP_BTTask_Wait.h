#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTTask_WaitBase.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "TPP_BTTask_Wait.generated.h"

UCLASS()
class TPP_01_API UTPP_BTTask_Wait : public UTPP_BTTask_WaitBase
{
	GENERATED_BODY()

public:
	UTPP_BTTask_Wait();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UPROPERTY(EditAnywhere, Category = "Wait")
	FBlackboardKeySelector WaitTimeKey;
};
