#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTTaskNode_Base.h"
#include "TPP_BTTaskNode_WaitUntilItemIsNotPossessed.generated.h"

UCLASS()
class TPP_01_API UTPP_BTTaskNode_WaitUntilItemIsNotPossessed : public UTPP_BTTaskNode_Base
{
	GENERATED_BODY()

public:
	UTPP_BTTaskNode_WaitUntilItemIsNotPossessed();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	UPROPERTY()
	TObjectPtr<class ATPP_InteractableItem> Item;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector ItemKey;
};
