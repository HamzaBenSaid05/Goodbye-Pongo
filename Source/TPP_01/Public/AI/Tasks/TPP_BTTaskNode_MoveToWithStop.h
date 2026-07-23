#pragma once

#include "CoreMinimal.h"
#include "TPP_BTTaskNode_MoveToCustomSpeed.h"
#include "TPP_BTTaskNode_MoveToWithStop.generated.h"

UCLASS()
class TPP_01_API UTPP_BTTaskNode_MoveToWithStop : public UTPP_BTTaskNode_MoveToCustomSpeed
{
	GENERATED_BODY()

public:
	UTPP_BTTaskNode_MoveToWithStop();

protected:
	UPROPERTY(EditAnywhere, Category = "AI|Moving")
	FBlackboardKeySelector bCanBeStopped;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
};