#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TPP_BTTask_EndInteract.generated.h"

UCLASS()
class TPP_01_API UTPP_BTTask_EndInteract : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTPP_BTTask_EndInteract();

protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	FBlackboardKeySelector TargetRotationKey;

};
