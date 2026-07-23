#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTTaskNode_Base.h"
#include "TPP_BTTask_SearchForInteract.generated.h"

UCLASS()
class TPP_01_API UTPP_BTTask_SearchForInteract : public UTPP_BTTaskNode_Base
{
	GENERATED_BODY()

public:
	UTPP_BTTask_SearchForInteract();

private:
	UPROPERTY(EditAnywhere, Category = "Location")
	FBlackboardKeySelector TargetLocationKey;
	UPROPERTY(EditAnywhere, Category = "Location")
	FBlackboardKeySelector PlayerKey;
	UPROPERTY(EditAnywhere, Category = "Object")
	FBlackboardKeySelector ItemToInteractKey;
	UPROPERTY(EditAnywhere, Category = "Rotation")
	FBlackboardKeySelector TargetRotationKey;
	UPROPERTY(EditAnywhere, Category = "Boolean")
	FBlackboardKeySelector IsInteractingKey;
	UPROPERTY(EditAnywhere)
	float InteractionLookRadius = 200.0f;
	UPROPERTY(EditAnywhere)
	FName BoneHeadName = "head";

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
