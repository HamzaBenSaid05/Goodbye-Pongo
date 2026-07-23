#include "AI/Services/TPP_BTService_MoveToCustomSpeedUntilArrive.h"

#include "AIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"

DEFINE_LOG_CATEGORY(LogMoveToCustomSpeedUntilArrive);

UTPP_BTService_MoveToCustomSpeedUntilArrive::UTPP_BTService_MoveToCustomSpeedUntilArrive()
{
	NodeName = TEXT("Move To Custom Speed Until Arrive");
	bNotifyCeaseRelevant = true;
	bNotifyBecomeRelevant = true;
}

void UTPP_BTService_MoveToCustomSpeedUntilArrive::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                           float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	if (!CacheObjects(OwnerComp))
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
			                                 "Cache Objects  Failed");
		}
#endif
		return;
	}
	AActor* TargetActor =
		Cast<AActor>(
		             CachedBlackboard->GetValueAsObject(EmotionalObjectKey.SelectedKeyName)
		            );

	if (!TargetActor) { return; }

	FVector TargetLocation = TargetActor->GetActorLocation();
	UE_LOG(LogMoveToCustomSpeedUntilArrive, Warning, TEXT("TargetLocation: %s"), *TargetLocation.ToString());
	UE_LOG(LogMoveToCustomSpeedUntilArrive, Warning, TEXT("PawnLocation: %s"), *CachedPongo->GetActorLocation().ToString());

	UNavigationSystemV1* NavSys =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (NavSys) { FPathFindingQuery Query; }
	FNavLocation Projected;

	bool bValid =
		UNavigationSystemV1::GetCurrent(GetWorld())
		->ProjectPointToNavigation(TargetLocation, Projected);

	UE_LOG(LogMoveToCustomSpeedUntilArrive, Warning,
	       TEXT("ProjectPointToNavigation: %s"),
	       bValid ? TEXT("VALID") : TEXT("INVALID"));
	if (!bValid)
	{
		const ANavigationData* NavData =
			NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
		FNavLocation NavLocation;

		NavSys->ProjectPointToNavigation(
		                                 TargetLocation,
		                                 NavLocation,
		                                 ProjectPointArea,
		                                 NavData
		                                );
		TargetLocation = NavLocation.Location;
#if WITH_EDITOR
		DrawDebugSphere(GetWorld(), TargetLocation, 25.f, 12, FColor::Green, false, 0.1f);
#endif
		bValid =
			UNavigationSystemV1::GetCurrent(GetWorld())
			->ProjectPointToNavigation(TargetLocation, Projected);

		UE_LOG(LogMoveToCustomSpeedUntilArrive, Warning,
		       TEXT("New ProjectPointToNavigation: %s"),
		       bValid ? TEXT("VALID") : TEXT("INVALID"));
	}

	float Distance = FVector::Dist2D(
	                               CachedPongo->GetActorLocation(),
	                               TargetLocation);
	UE_LOG(LogMoveToCustomSpeedUntilArrive, Warning, TEXT("Distance: %f"), Distance);

	if (Distance <= DistanceThreshold)
	{
		UE_LOG(LogMoveToCustomSpeedUntilArrive, Warning, TEXT("ARRIVED"));
		CachedBlackboard->SetValueAsBool(BooleanToSet.SelectedKeyName, true);
		return;
	}
	
	CachedBlackboard->SetValueAsBool(BooleanToSet.SelectedKeyName, false);

	const bool bTargetMoving = !TargetActor->GetVelocity().IsNearlyZero(1.0f);
	EPathFollowingStatus::Type MoveStatus = CachedPongoController->GetMoveStatus();
	bool bIsMoving = (MoveStatus == EPathFollowingStatus::Moving);
	const bool bIsStuck  = CachedPongo->GetVelocity().IsNearlyZero(1.0f);

	if (bTargetMoving && bIsMoving && !bIsStuck)
		return;

	if (!bIsMoving || bIsStuck)
	{
		FAIMoveRequest MoveReq;
		MoveReq.SetGoalLocation(TargetLocation);
		MoveReq.SetAcceptanceRadius(AcceptanceRadius);
		MoveReq.SetUsePathfinding(true);
		MoveReq.SetAllowPartialPath(true);
		MoveReq.SetProjectGoalLocation(true);
		EPathFollowingRequestResult::Type Result =
			CachedPongoController->MoveTo(MoveReq);

		UE_LOG(LogMoveToCustomSpeedUntilArrive, Warning, TEXT("MoveTo Result = %d"), (int32)Result);
	}
}

void UTPP_BTService_MoveToCustomSpeedUntilArrive::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
	CachedBlackboard->SetValueAsBool(BooleanToSet.SelectedKeyName, false);
}

void UTPP_BTService_MoveToCustomSpeedUntilArrive::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	CachedBlackboard->SetValueAsBool(BooleanToSet.SelectedKeyName, false);
}
