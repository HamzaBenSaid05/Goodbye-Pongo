#include "AI/Tasks/TPP_BTTask_MoveToCustomSpeedUntilArrive.h"

#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

UTPP_BTTask_MoveToCustomSpeedUntilArrive::UTPP_BTTask_MoveToCustomSpeedUntilArrive()
{
	bNotifyTick = true;
	NodeName = "Move To Custom Speed Until Arrive";
}

EBTNodeResult::Type UTPP_BTTask_MoveToCustomSpeedUntilArrive::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (Result == EBTNodeResult::Failed)
		return EBTNodeResult::InProgress;

	return Result;
}

void UTPP_BTTask_MoveToCustomSpeedUntilArrive::TickTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!CacheObjects(OwnerComp)) return;

	FVector TargetLocation = Cast<AActor>(CachedBlackboard->GetValueAsObject(EmotionalObjectKey.SelectedKeyName))->
		GetActorLocation();

	UE_LOG(LogTemp, Warning, TEXT("TargetLocation: %s"), *TargetLocation.ToString());
	UE_LOG(LogTemp, Warning, TEXT("PawnLocation: %s"), *CachedPawn->GetActorLocation().ToString());
	UE_LOG(LogTemp, Warning, TEXT("KeyName: %s"), *BlackboardKey.SelectedKeyName.ToString());

	float Distance = FVector::Dist(CachedPawn->GetActorLocation(), TargetLocation);
	UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);

	if (Distance <= AcceptanceRadius)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	UNavigationSystemV1* NavSys =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(
			CachedPawn->GetWorld());

	if (!NavSys)
	{
		UE_LOG(LogTemp, Warning, TEXT("No NavigationSystem found"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	FNavLocation NavLocation;
	const ANavigationData* NavData =
		NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	const bool bOnNavMesh = NavSys->ProjectPointToNavigation(
		TargetLocation,
		NavLocation,
		FVector(300.f, 300.f, 300.f),
		NavData
	);
	if (bOnNavMesh)
	{
		TargetLocation = NavLocation.Location;
		FAIMoveRequest MoveReq;
		MoveReq.SetGoalLocation(TargetLocation);
		MoveReq.SetAcceptanceRadius(AcceptanceRadius);
		MoveReq.SetUsePathfinding(true);
		MoveReq.SetAllowPartialPath(true);
		MoveReq.SetProjectGoalLocation(true);

		CachedController->MoveTo(MoveReq);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Target location is not on navmesh"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}
