#pragma once

#include "CoreMinimal.h"
#include "AI/Tasks/Base/TPP_BTService_Base.h"
#include "TPP_BTService_CheckDistanceToActor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCheckDistanceToActor, Log, All);

UCLASS()
class TPP_01_API UTPP_BTService_CheckDistanceToActor : public UTPP_BTService_Base
{
	GENERATED_BODY()

public:
	UTPP_BTService_CheckDistanceToActor();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Distance")
	float TriggetDistance = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ActorToCheckDistanceTo;
	UPROPERTY(Transient)
	bool bSetBooleanOnSuccess = false;
	UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (EditCondition = "bSetBooleanOnSuccess"))
	FBlackboardKeySelector BooleanToSet;
};
