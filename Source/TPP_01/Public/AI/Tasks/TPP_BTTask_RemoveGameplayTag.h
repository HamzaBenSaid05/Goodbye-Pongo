#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTTaskNode_Base.h"
#include "TPP_BTTask_RemoveGameplayTag.generated.h"

UCLASS()
class TPP_01_API UTPP_BTTask_RemoveGameplayTag : public UTPP_BTTaskNode_Base
{
	GENERATED_BODY()
	
private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UPROPERTY(EditAnywhere, Category="AI|GameplayTags")
	FGameplayTag GameplayTagToRemove;
};
