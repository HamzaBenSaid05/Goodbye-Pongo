#include "AI/Tasks/Decorator/TPP_BTDecorator_CheckDistance.h"

UTPP_BTDecorator_CheckDistance::UTPP_BTDecorator_CheckDistance() { NodeName = "Check Distance"; }

bool UTPP_BTDecorator_CheckDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!Super::CalculateRawConditionValue(OwnerComp, NodeMemory))
		return false;

	if (const AActor* TargetActor = Cast<AActor>(CachedBlackboard->GetValueAsObject(BlackboardKey.SelectedKeyName)))
	{
		const float DistanceSquared = (CachedPawn->GetActorLocation() - TargetActor->GetActorLocation()).SizeSquared();
		return DistanceSquared <= FMath::Square(TriggetDistance);
	}
	return false;
}
