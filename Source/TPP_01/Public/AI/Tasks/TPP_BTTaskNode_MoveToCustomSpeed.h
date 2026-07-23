#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTTaskNode_MoveToBase.h"
#include "TPP_BTTaskNode_MoveToCustomSpeed.generated.h"

UCLASS()
class TPP_01_API UTPP_BTTaskNode_MoveToCustomSpeed : public UTPP_BTTaskNode_MoveToBase
{
	GENERATED_BODY()

public:
	UTPP_BTTaskNode_MoveToCustomSpeed();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	UPROPERTY(EditAnywhere, Category="AI|State")
	FBlackboardKeySelector PongoStateKey;
	UPROPERTY(EditAnywhere, Category = "AI|State")
	FBlackboardKeySelector bIsMovingKey;
};
