#pragma once

#include "CoreMinimal.h"
#include "TPP_BTTaskNode_MoveToCustomSpeed.h"
#include "TPP_BTTask_MoveToCustomSpeedUntilArrive.generated.h"

UCLASS()
class TPP_01_API UTPP_BTTask_MoveToCustomSpeedUntilArrive : public UTPP_BTTaskNode_MoveToCustomSpeed
{
	GENERATED_BODY()

public:
	UTPP_BTTask_MoveToCustomSpeedUntilArrive();
	UPROPERTY(EditAnywhere, Category = "Move")
	float AcceptanceRadius = 50.f;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector EmotionalObjectKey;
	
private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
