#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTTaskNode_Base.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "TPP_BTTaskNode_ComeHereMoveTo.generated.h"

UCLASS()
class UTPP_BTTaskNode_ComeHereMoveTo : public UTPP_BTTaskNode_Base
{
	GENERATED_BODY()

public:
	UTPP_BTTaskNode_ComeHereMoveTo();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsMovingKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector bCanBeStopped;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsComeHereCommandKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MinDistanceKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector FollowDurationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ObservedBlackboardValueToleranceKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MarginOfTolleranceKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetLocationKey;

	UPROPERTY(EditAnywhere, Category = "Navigation")
	FVector QueryExtent = FVector(150.f, 150.f, 0.f);

	UPROPERTY(EditAnywhere, Category = "Navigation")
	float MoveUpdateCooldown = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Navigation")
	float TolleranceDistance = 2.f;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationInterpSpeed = 5.f;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	float StopVelocityThreshold = 10.f;

private:
	void StartStopTimer(float Duration);
	void StopComeHere();
	void ClearStopTimer();

	void UpdateTargetLocationOnBlackboard() const;

	FTimerHandle TimerHandle;
	FVector LastMoveDestination = FVector::ZeroVector;
	bool bIsTimerStarted = false;
	float TimeSinceLastMove = 0.f;
	float AcceptableRadius;
};
