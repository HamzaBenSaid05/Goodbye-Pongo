#pragma once

#include "CoreMinimal.h"
#include "AI/Tasks/Base/TPP_BTService_Base.h"
#include "TPP_BTService_RotateTo.generated.h"

UCLASS()
class TPP_01_API UTPP_BTService_RotateTo : public UTPP_BTService_Base
{
	GENERATED_BODY()

public:
	UTPP_BTService_RotateTo();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationThreshold = 10.f;
	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationInterpSpeed = 5.f;
	UPROPERTY(EditAnywhere, Category = "Rotation", meta = (Unit = "degrees"))
	float AlignmentThreshold = 10.f;
};
