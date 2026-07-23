#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTTaskNode_Base.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TPP_BTTaskNode_CalculateEscapeRange.generated.h"

UCLASS()
class TPP_01_API UTPP_BTTaskNode_CalculateEscapeRange : public UTPP_BTTaskNode_Base
{
	GENERATED_BODY()
public:
	UTPP_BTTaskNode_CalculateEscapeRange();
	
private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UPROPERTY(EditAnywhere, Category="AI")
	FBlackboardKeySelector EmotionalStateObjectKey;
	UPROPERTY(EditAnywhere, Category="AI")
	FBlackboardKeySelector OuterRangeKey;
	UPROPERTY(EditAnywhere, Category="AI")
	float EscapeOffset = 200.f;
};
