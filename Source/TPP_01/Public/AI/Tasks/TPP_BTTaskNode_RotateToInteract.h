// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TPP_BTTaskNode_RotateToInteract.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_BTTaskNode_RotateToInteract : public UBTTaskNode
{
	GENERATED_BODY()
	
	UTPP_BTTaskNode_RotateToInteract();
public:
	UPROPERTY(EditAnywhere, Category = "Rotation")
	FBlackboardKeySelector TargetRotationKey;
	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationSpeed;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


};
