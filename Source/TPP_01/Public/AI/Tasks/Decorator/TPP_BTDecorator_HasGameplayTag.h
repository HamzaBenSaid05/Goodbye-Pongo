#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTDecorator_Base.h"
#include "TPP_BTDecorator_HasGameplayTag.generated.h"

UCLASS()
class TPP_01_API UTPP_BTDecorator_HasGameplayTag : public UTPP_BTDecorator_Base
{
	GENERATED_BODY()

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	UPROPERTY(EditAnywhere, Category="AI|GameplayTags")
	FGameplayTag GameplayTagToCheck;
};
