#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AI/AIController/TPP_PongoController.h"
#include "Characters/Pongo/TPP_Pongo.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TPP_BTTaskNode_Base.generated.h"

UCLASS(Abstract)
class TPP_01_API UTPP_BTTaskNode_Base : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	mutable TWeakObjectPtr<ATPP_PongoController> CachedPongoController;
	mutable TWeakObjectPtr<ATPP_Pongo> CachedPongo;
	mutable TWeakObjectPtr<UBlackboardComponent> CachedBlackboard;
	mutable TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

	bool CacheObjects(UBehaviorTreeComponent& OwnerComp) const;
};
