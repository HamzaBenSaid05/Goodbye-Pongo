#pragma once

#include "CoreMinimal.h"
#include "AI/Tasks/Base/TPP_BTService_Base.h"
#include "TPP_BTService_UpdateStrollOfFear.generated.h"

UCLASS()
class TPP_01_API UTPP_BTService_UpdateStrollOfFear : public UTPP_BTService_Base
{
	GENERATED_BODY()
	
public:
	UTPP_BTService_UpdateStrollOfFear();
	
protected:
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector IsMovingKey;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector CanStrollFearKey;

	UPROPERTY(EditAnywhere, Category="SafePoint")
	FBlackboardKeySelector SafePointSearchRadiusKey;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector HasNearbySafePointKey;

	UPROPERTY(EditAnywhere, Category="Fear", meta=(ClampMin="0.0"))
	float DistanceFromActor = 500.f;

	UPROPERTY(EditAnywhere, Category="Fear", meta=(ClampMin="0.0"))
	float DelayBeforeCanStroll = 5.f;


	UPROPERTY(EditAnywhere, Category="SafePoint")
	TEnumAsByte<ECollisionChannel> SafePointCollisionChannel = ECC_GameTraceChannel1;

	UPROPERTY(EditAnywhere, Category="SafePoint")
	TSubclassOf<AActor> SafePointClass;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	
private:
	FTimerHandle StrollFearTimerHandle;

	void ClearStrollFearTimer();
	void StartStrollFearTimer();

	UFUNCTION()
	void OnStrollFearTimerElapsed();

	bool UpdateNearbySafePoint() const;

	void SetBlackboardBool(const FBlackboardKeySelector& Key, bool bValue) const;
	bool GetBlackboardBool(const FBlackboardKeySelector& Key) const;
	AActor* GetBlackboardActor(const FBlackboardKeySelector& Key) const;
};
