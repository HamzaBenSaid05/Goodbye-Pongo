// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TPP_BTTask_SetEmotionalStatePoint.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_BTTask_SetEmotionalStatePoint : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UTPP_BTTask_SetEmotionalStatePoint();
	UPROPERTY(EditAnywhere, Category = "Location")
	FBlackboardKeySelector EmotionalStateObject;
	UPROPERTY(EditAnywhere, Category = "Location")
	FBlackboardKeySelector LocationStateObject;
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
