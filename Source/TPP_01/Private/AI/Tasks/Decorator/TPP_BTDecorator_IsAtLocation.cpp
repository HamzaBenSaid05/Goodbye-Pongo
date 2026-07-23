#include "AI/Tasks/Decorator/TPP_BTDecorator_IsAtLocation.h"

UTPP_BTDecorator_IsAtLocation::UTPP_BTDecorator_IsAtLocation()
{
	NodeName = "Is At Location";
}

bool UTPP_BTDecorator_IsAtLocation::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                               uint8* NodeMemory) const
{
	if (!CacheObjects(OwnerComp))
	{
		return false;
	}
	
	AActor* TargetActor = Cast<AActor>(CachedBlackboard->GetValueAsObject(LocationKey.SelectedKeyName));
	if (!TargetActor) return false;
	
	if (DistanceType == Distance2D)
	{
		return FVector::DistSquared2D(CachedPawn->GetActorLocation(), TargetActor->GetActorLocation()) <= FMath::Square(AcceptanceRadius);
	}
	if (DistanceType == Distance3D)
	{
		return FVector::DistSquared(CachedPawn->GetActorLocation(), TargetActor->GetActorLocation()) <= FMath::Square(AcceptanceRadius);
	}
	return EBTNodeResult::Failed;
}
