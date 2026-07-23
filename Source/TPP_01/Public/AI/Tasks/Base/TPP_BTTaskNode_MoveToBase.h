#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TPP_BTTaskNode_MoveToBase.generated.h"

UCLASS()
class TPP_01_API UTPP_BTTaskNode_MoveToBase : public UBTTask_MoveTo
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	mutable TWeakObjectPtr<AAIController> CachedController;
	mutable TWeakObjectPtr<APawn> CachedPawn;
	mutable TWeakObjectPtr<UBlackboardComponent> CachedBlackboard;

	bool CacheObjects(UBehaviorTreeComponent& OwnerComp) const;
};
