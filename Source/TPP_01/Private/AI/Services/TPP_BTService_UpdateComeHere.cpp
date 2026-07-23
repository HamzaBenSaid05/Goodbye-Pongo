#include "AI/Services/TPP_BTService_UpdateComeHere.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

UTPP_BTService_UpdateComeHere::UTPP_BTService_UpdateComeHere()
{
	NodeName = TEXT("Update Come Here");
	Interval = 0.2f;
	RandomDeviation = 0.0f;
}

void UTPP_BTService_UpdateComeHere::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
	
	AActor* TargetActor = Cast<AActor>(CachedBlackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
			                                 "Target Null");
		}
#endif
		ClearStopTimer();
		return;
	}

	const FVector ControlledPawnLocation = CachedPongo->GetActorLocation();
	FVector DesiredLocation = TargetActor->GetActorLocation();

	// Check if desired location is on navmesh, if not project it to navmesh
	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		FNavLocation ProjectedLocation;
		bool bPointToNav = NavSys->ProjectPointToNavigation(
		                                                    DesiredLocation,
		                                                    ProjectedLocation
		                                                   );

		FVector Current = CachedBlackboard->GetValueAsVector(TargetLocationKey.SelectedKeyName);

		if (bPointToNav)
		{
			if (FVector::DistSquared(Current, ProjectedLocation.Location) > TolleranceDistance)
			{
			
				CachedBlackboard->SetValueAsVector(TargetLocationKey.SelectedKeyName, ProjectedLocation.Location);
				// draw debug sphere at project location
	#if WITH_EDITOR
				if (GEngine)
				{
					DrawDebugSphere(GetWorld(), ProjectedLocation.Location, 25.f, 12, FColor::Green, false, 0.1f);
				}
	#endif
			}
		}
		else
		{
#if WITH_EDITOR
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Red,
				                                 "Target Is Not On Nav");
			}
#endif
			if (NavSys->ProjectPointToNavigation(DesiredLocation, ProjectedLocation, QueryExtent))
			{
				if (FVector::DistSquared(Current, ProjectedLocation.Location) > TolleranceDistance)
				{
					CachedBlackboard->SetValueAsVector(TargetLocationKey.SelectedKeyName, ProjectedLocation.Location);
					// draw debug sphere at project location
#if WITH_EDITOR
					if (GEngine)
					{
						DrawDebugSphere(GetWorld(), ProjectedLocation.Location, 25.f, 12, FColor::Green, false, 0.1f);
					}
#endif
				}
			}
		}
	}

	// If timer started no need to check distance until timer is cleared
	if (bIsTimerStarted)
		return;

	const float MinDistance = CachedBlackboard->GetValueAsFloat(MinDistanceKey.SelectedKeyName);
	const float DistanceSq = FVector::DistSquaredXY(ControlledPawnLocation, TargetActor->GetActorLocation());

	if (DistanceSq < FMath::Square(MinDistance))
	{
		const float FollowDuration = CachedBlackboard->GetValueAsFloat(FollowDurationKey.SelectedKeyName);
		StartStopTimer(FollowDuration);
	}
}

void UTPP_BTService_UpdateComeHere::StartStopTimer(float Duration)
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
	                      &UTPP_BTService_UpdateComeHere::StopComeHere,
	                      Duration,
	                      false
	                     );
}

void UTPP_BTService_UpdateComeHere::ClearStopTimer()
{
	if (GetWorld()) { GetWorld()->GetTimerManager().ClearTimer(TimerHandle); }
}

void UTPP_BTService_UpdateComeHere::StopComeHere()
{
	if (!CachedPongoController.IsValid() || !CachedBlackboard.IsValid()) { return; }

	CachedPongoController->StopMovement();

	if (CachedBlackboard.Get())
		CachedBlackboard->SetValueAsBool(IsComeHereInteractionKey.SelectedKeyName, false);
	ClearStopTimer();
	bIsTimerStarted = false;
}
