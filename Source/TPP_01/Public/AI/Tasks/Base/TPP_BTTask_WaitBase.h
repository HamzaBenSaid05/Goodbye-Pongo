#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TPP_BTTask_WaitBase.generated.h"

UCLASS(Abstract)
class TPP_01_API UTPP_BTTask_WaitBase : public UBTTask_Wait
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	mutable TWeakObjectPtr<AAIController> CachedController;
	mutable TWeakObjectPtr<APawn> CachedPawn;
	mutable TWeakObjectPtr<UBlackboardComponent> CachedBlackboard;
	
	bool CacheObjects(UBehaviorTreeComponent& OwnerComp) const;
};
