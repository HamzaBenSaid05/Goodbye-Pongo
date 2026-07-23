// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TPP_BTTaskVegetationShooting.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_BTTaskVegetationShooting : public UBTTaskNode
{
	GENERATED_BODY()
	
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	public:
	UPROPERTY(EditAnywhere, Category = "Actor")
	FBlackboardKeySelector TargetActorKey;
};