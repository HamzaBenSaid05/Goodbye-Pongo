#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "TPP_BTDecorator_Base.generated.h"

UCLASS(Abstract)
class TPP_01_API UTPP_BTDecorator_Base : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	mutable TWeakObjectPtr<AAIController> CachedController;
	mutable TWeakObjectPtr<APawn> CachedPawn;
	mutable TWeakObjectPtr<UBlackboardComponent> CachedBlackboard;
	
	bool CacheObjects(UBehaviorTreeComponent& OwnerComp) const;
};
