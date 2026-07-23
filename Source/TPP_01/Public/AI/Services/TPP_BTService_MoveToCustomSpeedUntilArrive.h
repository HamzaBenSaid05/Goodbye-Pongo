#pragma once

#include "CoreMinimal.h"
#include "AI/Tasks/Base/TPP_BTService_Base.h"
#include "TPP_BTService_MoveToCustomSpeedUntilArrive.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMoveToCustomSpeedUntilArrive, Log, All);

UCLASS()
class TPP_01_API UTPP_BTService_MoveToCustomSpeedUntilArrive : public UTPP_BTService_Base
{
	GENERATED_BODY()

public:
	UTPP_BTService_MoveToCustomSpeedUntilArrive();
	UPROPERTY(EditAnywhere, Category = "Distance")
	float DistanceThreshold = 150.f;
	UPROPERTY(EditAnywhere, Category = "Acceptance")
	float AcceptanceRadius = 50.f;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector EmotionalObjectKey;
	UPROPERTY(EditAnywhere, Category = "Projection")
	FVector ProjectPointArea = FVector(150.f, 150.f, 300.f);
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BooleanToSet;
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
