#pragma once

#include "CoreMinimal.h"
#include "AI/Tasks/Base/TPP_BTService_Base.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "TPP_BTService_UpdateComeHere.generated.h"

UCLASS()
class TPP_01_API UTPP_BTService_UpdateComeHere : public UTPP_BTService_Base
{
	GENERATED_BODY()

public:
	UTPP_BTService_UpdateComeHere();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	UFUNCTION()
	void StopComeHere();
	
	void ClearStopTimer();
	void StartStopTimer(float Duration);

	// Actor To Follow
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	// Location to update
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetLocationKey;

	// Min distance to trigger target arrived
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MinDistanceKey;

	// Timer when condition is satisfing
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector FollowDurationKey;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsComeHereInteractionKey;

	UPROPERTY(EditAnywhere, Category = "Navigation")
	FVector QueryExtent = FVector(150.f, 150.f, 0.f);
	
	UPROPERTY(EditAnywhere, Category = "Navigation")
	float TolleranceDistance = 2.f;
	
private:
	FTimerHandle TimerHandle;
	
	bool bIsTimerStarted = false;
	
};
