#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTTaskNode_Base.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "TPP_BTTask_PlayAnimationMontage.generated.h"

UCLASS()
class TPP_01_API UTPP_BTTask_PlayAnimationMontage : public UTPP_BTTaskNode_Base
{
	GENERATED_BODY()

public:
	UTPP_BTTask_PlayAnimationMontage();

protected:
	UPROPERTY(Category = Node, EditAnywhere)
	FValueOrBBKey_Object AnimationToPlay = TObjectPtr<UAnimationAsset>();

	UPROPERTY(Category = Node, EditAnywhere)
	FName SlotName = TEXT("DefaultSlot");

	UPROPERTY(Category = Node, EditAnywhere, DisplayName = Looping)
	FValueOrBBKey_Bool bLooping = false;

	UPROPERTY(Category = Node, EditAnywhere, DisplayName = NonBlocking)
	FValueOrBBKey_Bool bNonBlocking = false;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CachedSkelMesh;

	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedMontage;

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	EAnimationMode::Type PreviousAnimationMode;

	FTimerDelegate TimerDelegate;
	FTimerHandle TimerHandle;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};