#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTDecorator_Base.h"
#include "TPP_BTDecorator_CheckDistance.generated.h"

UCLASS()
class TPP_01_API UTPP_BTDecorator_CheckDistance : public UTPP_BTDecorator_Base
{
	GENERATED_BODY()

public:
	UTPP_BTDecorator_CheckDistance();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Distance", meta = (AllowPrivateAccess="true"))
	float TriggetDistance = 1000.f;
};
