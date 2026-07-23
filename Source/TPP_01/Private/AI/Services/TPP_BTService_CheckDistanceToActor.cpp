#include "AI/Services/TPP_BTService_CheckDistanceToActor.h"

#include "BehaviorTree/BlackboardComponent.h"

DEFINE_LOG_CATEGORY(LogCheckDistanceToActor);

UTPP_BTService_CheckDistanceToActor::UTPP_BTService_CheckDistanceToActor()
{
	NodeName = "CheckDistanceToActor";
	bNotifyCeaseRelevant = true;
	bNotifyBecomeRelevant = true;
}

void UTPP_BTService_CheckDistanceToActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                   float DeltaSeconds)
{
	if (!CacheObjects(OwnerComp))
	{
		return;
	}

	if (const AActor* TargetActor = Cast<AActor>(
		CachedBlackboard->GetValueAsObject(ActorToCheckDistanceTo.SelectedKeyName)))
	{
		//const float DistanceSquared = FVector::DistSquared(CachedPawn->GetActorLocation(), TargetActor->GetActorLocation());
		const float Distance = FVector::Dist(CachedPongo->GetActorLocation(), TargetActor->GetActorLocation());
#if WITH_EDITOR
		UE_LOG(LogCheckDistanceToActor, Warning,
		       TEXT("Distance check distance: %f"), Distance);
#endif
		//if (Distance <= FMath::Square(TriggetDistance))
		if (Distance <= TriggetDistance)
		{
			CachedBlackboard->SetValueAsBool(BooleanToSet.SelectedKeyName, true);
		}
		else
		{
			CachedBlackboard->SetValueAsBool(BooleanToSet.SelectedKeyName, false);
		}
	}
	else
	{
		CachedBlackboard->SetValueAsBool(BooleanToSet.SelectedKeyName, false);
	}
}

void UTPP_BTService_CheckDistanceToActor::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
	CachedBlackboard->SetValueAsBool(BooleanToSet.SelectedKeyName, false);
}

void UTPP_BTService_CheckDistanceToActor::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	CachedBlackboard->SetValueAsBool(BooleanToSet.SelectedKeyName, false);
}
