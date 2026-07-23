#include "AI/Tasks/TPP_BTTaskNode_ComeHereMoveTo.h"

#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

UTPP_BTTaskNode_ComeHereMoveTo::UTPP_BTTaskNode_ComeHereMoveTo()
{
	NodeName = TEXT("Update Come Here");
	bNotifyTick = true;
}

EBTNodeResult::Type UTPP_BTTaskNode_ComeHereMoveTo::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp))
		return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(
	                                   CachedBlackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
		return EBTNodeResult::Failed;

	// Set Idle Velocity
	if (ITPP_AIAction* AIAction = Cast<ITPP_AIAction>(CachedPongo.Get())) { AIAction->SetSpeedBasedOnState(static_cast<uint8>(ETPP_PongoState::Idle)); }

	bIsTimerStarted = false;
	LastMoveDestination = FVector::ZeroVector;
	ClearStopTimer();
	TimeSinceLastMove = MoveUpdateCooldown;

	UpdateTargetLocationOnBlackboard();

	FVector TargetLocation = TargetActor->GetActorLocation();
	UNavigationSystemV1* NavSys =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys) return EBTNodeResult::Failed;

	FNavLocation ProjectedLocation;
	if (!NavSys->ProjectPointToNavigation(TargetLocation, ProjectedLocation))
		if (!NavSys->ProjectPointToNavigation(
		                                      TargetLocation, ProjectedLocation, QueryExtent))
			return EBTNodeResult::Failed;

	LastMoveDestination = ProjectedLocation.Location;

	FAIMoveRequest MoveReq;
	AcceptableRadius =
		CachedBlackboard->GetValueAsFloat(MarginOfTolleranceKey.SelectedKeyName);
	MoveReq.SetGoalLocation(ProjectedLocation.Location);
	MoveReq.SetAcceptanceRadius(AcceptableRadius);
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(true);
	MoveReq.SetProjectGoalLocation(true);

	CachedPongoController->MoveTo(MoveReq);

	return EBTNodeResult::InProgress;
}

void UTPP_BTTaskNode_ComeHereMoveTo::TickTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!CacheObjects(OwnerComp))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (CachedPongoController.IsValid())
	{
		UPathFollowingComponent* PathComp =
			CachedPongoController->GetPathFollowingComponent();

		const bool bIsMoving =
			PathComp &&
			PathComp->GetStatus() == EPathFollowingStatus::Moving;

		CachedBlackboard->SetValueAsBool(
		                                 IsMovingKey.SelectedKeyName,
		                                 bIsMoving);
		CachedBlackboard->SetValueAsBool(
		                                 bCanBeStopped.SelectedKeyName,
		                                 bIsMoving);
	}

	// Updated target location 
	UpdateTargetLocationOnBlackboard();

	// Rotate toward target only if we are still
	UCharacterMovementComponent* CharMovement = CachedPongo->GetCharacterMovement();
	if (CachedPongo.Get() 
		&& CharMovement 
		&& !CharMovement->IsFalling()      
		&& !CharMovement->IsFlying()
		&& CachedPongo->GetVelocity().SizeSquared() < FMath::Square(StopVelocityThreshold))
	{
		AActor* TargetActor = Cast<AActor>(
			CachedBlackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
		if (TargetActor)
		{
			const FVector ToTarget =
				(TargetActor->GetActorLocation() - CachedPongo->GetActorLocation()).GetSafeNormal();

			if (!ToTarget.IsNearlyZero())
			{
				const FRotator NewRot = FMath::RInterpTo(
					CachedPongo->GetActorRotation(),
					ToTarget.Rotation(),
					DeltaSeconds,
					RotationInterpSpeed);

				CachedPongo->SetActorRotation(NewRot);
			}
		}
	}

	if (!CachedBlackboard->GetValueAsBool(IsComeHereCommandKey.SelectedKeyName))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	AActor* TargetActor = Cast<AActor>(
	                                   CachedBlackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const float MinDistance =
		CachedBlackboard->GetValueAsFloat(MinDistanceKey.SelectedKeyName);
	const float DistSq = FVector::DistSquared(
	                                          CachedPongo->GetActorLocation(), TargetActor->GetActorLocation());

	if (DistSq < FMath::Square(MinDistance) && !bIsTimerStarted)
	{
		StartStopTimer(
		               CachedBlackboard->GetValueAsFloat(FollowDurationKey.SelectedKeyName));
	}

	FVector TargetLocation = TargetActor->GetActorLocation();
	UNavigationSystemV1* NavSys =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys) return;

	FNavLocation ProjectedLocation;
	if (!NavSys->ProjectPointToNavigation(TargetLocation, ProjectedLocation))
		if (!NavSys->ProjectPointToNavigation(
		                                      TargetLocation, ProjectedLocation, QueryExtent))
			return;

	const float ToleranceSq = FMath::Square(
	                                        CachedBlackboard->GetValueAsFloat(
	                                                                          ObservedBlackboardValueToleranceKey.SelectedKeyName));
	TimeSinceLastMove += DeltaSeconds;
	if (FVector::DistSquared(LastMoveDestination, ProjectedLocation.Location) > ToleranceSq
		&& TimeSinceLastMove >= MoveUpdateCooldown)
	{
		LastMoveDestination = ProjectedLocation.Location;
		TimeSinceLastMove = 0.f; // reset cooldown

		FAIMoveRequest MoveReq;
		MoveReq.SetGoalLocation(ProjectedLocation.Location);
		MoveReq.SetAcceptanceRadius(AcceptableRadius);
		MoveReq.SetUsePathfinding(true);
		MoveReq.SetAllowPartialPath(true);
		MoveReq.SetProjectGoalLocation(true);

		CachedPongoController->MoveTo(MoveReq);
	}
}

EBTNodeResult::Type UTPP_BTTaskNode_ComeHereMoveTo::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp))
	{
		return Super::AbortTask(OwnerComp, NodeMemory);
	}
	CachedBlackboard->SetValueAsBool(
	                                 IsMovingKey.SelectedKeyName,
	                                 false);
	CachedBlackboard->SetValueAsBool(
								 bCanBeStopped.SelectedKeyName,
								 false);
	ClearStopTimer();
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UTPP_BTTaskNode_ComeHereMoveTo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	CachedBlackboard->SetValueAsBool(
	                                 IsMovingKey.SelectedKeyName,
	                                 false);
	CachedBlackboard->SetValueAsBool(
							 bCanBeStopped.SelectedKeyName,
							 false);
	ClearStopTimer();
}

void UTPP_BTTaskNode_ComeHereMoveTo::StartStopTimer(float Duration)
{
	if (!CachedPongoController.IsValid())
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
			                                 "AI Controller Null");
		}
#endif
		return;
	}
	UWorld* World = CachedPongo->GetWorld();
	if (!World)
		return;
	FTimerManager& TimerManager = World->GetTimerManager();
	if (TimerManager.TimerExists(TimerHandle) || TimerManager.IsTimerActive(TimerHandle))
		return;
	bIsTimerStarted = true;
	TimerManager.SetTimer(
	                      TimerHandle,
	                      this,
	                      &UTPP_BTTaskNode_ComeHereMoveTo::StopComeHere,
	                      Duration,
	                      false
	                     );
}

void UTPP_BTTaskNode_ComeHereMoveTo::StopComeHere()
{
	if (!CachedPongoController.IsValid() || !CachedBlackboard.IsValid()) { return; }

	CachedPongoController->StopMovement();

	if (CachedBlackboard.Get())
		CachedBlackboard->SetValueAsBool(IsComeHereCommandKey.SelectedKeyName, false);
	ClearStopTimer();
	bIsTimerStarted = false;
}

void UTPP_BTTaskNode_ComeHereMoveTo::ClearStopTimer()
{
	UWorld* World = GetWorld();
	if (World && World->GetTimerManager().TimerExists(TimerHandle)) { World->GetTimerManager().ClearTimer(TimerHandle); }
	bIsTimerStarted = false;
}

void UTPP_BTTaskNode_ComeHereMoveTo::UpdateTargetLocationOnBlackboard() const
{
	if (!CachedBlackboard.IsValid()) return;

	AActor* TargetActor = Cast<AActor>(
	                                   CachedBlackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor) return;

	UNavigationSystemV1* NavSys =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys) return;

	FVector DesiredLocation = TargetActor->GetActorLocation();
	FNavLocation ProjectedLocation;

	// Try without extent and if not on nav try with correction
	bool bProjected = NavSys->ProjectPointToNavigation(DesiredLocation, ProjectedLocation);
	if (!bProjected)
	{
#if WITH_EDITOR
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Red, "Target Is Not On Nav");
#endif
		bProjected = NavSys->ProjectPointToNavigation(
		                                              DesiredLocation, ProjectedLocation, QueryExtent);
	}

	if (!bProjected) return;

	FVector Current =
		CachedBlackboard->GetValueAsVector(TargetLocationKey.SelectedKeyName);

	if (FVector::DistSquared(Current, ProjectedLocation.Location) > TolleranceDistance)
	{
		CachedBlackboard->SetValueAsVector(
		                                   TargetLocationKey.SelectedKeyName, ProjectedLocation.Location);

#if WITH_EDITOR
		DrawDebugSphere(GetWorld(), ProjectedLocation.Location,
		                25.f, 12, FColor::Green, false, 0.1f);
#endif
	}
}
