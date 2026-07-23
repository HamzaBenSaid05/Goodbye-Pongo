#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTDecorator_Base.h"
#include "TPP_BTDecorator_IsAtLocation.generated.h"

UENUM()
enum class EDistanceType : uint8
{
	Distance3D,
	Distance2D
};

UCLASS()
class TPP_01_API UTPP_BTDecorator_IsAtLocation : public UTPP_BTDecorator_Base
{
	GENERATED_BODY()

public:
	UTPP_BTDecorator_IsAtLocation();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

private:
	using enum EDistanceType;
	UPROPERTY(EditAnywhere, Category = "Distance|Type")
	EDistanceType DistanceType = EDistanceType::Distance3D;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LocationKey;
	UPROPERTY(EditAnywhere, Category = "Distance|Radius")
	float AcceptanceRadius = 50.f;
};
