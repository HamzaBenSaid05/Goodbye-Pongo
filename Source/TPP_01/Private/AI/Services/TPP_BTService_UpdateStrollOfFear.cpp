#include "AI/Services/TPP_BTService_UpdateStrollOfFear.h"

#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"

UTPP_BTService_UpdateStrollOfFear::UTPP_BTService_UpdateStrollOfFear()
{
	NodeName = "HandleStrollOfFear";
	Interval = 0.1f;
	RandomDeviation = 0.3f;
	bNotifyCeaseRelevant = true;
}

void UTPP_BTService_UpdateStrollOfFear::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                 float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!CacheObjects(OwnerComp))
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange,
			                                 "Cache Objects  Failed");
		}
#endif
		ClearStrollFearTimer();
		SetBlackboardBool(CanStrollFearKey, false);
		return;
	}

	const bool bHasNearbySafePoint = UpdateNearbySafePoint();

	if (!bHasNearbySafePoint)
	{
		ClearStrollFearTimer();
		SetBlackboardBool(CanStrollFearKey, false);
		return;
	}

	const bool bIsMoving = GetBlackboardBool(IsMovingKey);
	if (bIsMoving)
		return;

	AActor* TargetActor = GetBlackboardActor(TargetActorKey);
	if (!IsValid(TargetActor))
		return;

	const FVector PawnLocation = CachedPongo->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();

	const float DistSq = FVector::DistSquared(PawnLocation, TargetLocation);
	const float MaxDistSq = FMath::Square(DistanceFromActor);
	const bool bInsideMaxDistance = DistSq <= MaxDistSq;
	const bool bCanStrollFear = GetBlackboardBool(CanStrollFearKey);
	if (bInsideMaxDistance)
	{
		ClearStrollFearTimer();
		if (bCanStrollFear)
			SetBlackboardBool(CanStrollFearKey, false);
		return;
	}

	StartStrollFearTimer();
}

void UTPP_BTService_UpdateStrollOfFear::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
	ClearStrollFearTimer();
}

void UTPP_BTService_UpdateStrollOfFear::ClearStrollFearTimer()
{
	if (!CachedPongo.IsValid())
		return;

	UWorld* World = CachedPongo->GetWorld();
	if (!World)
		return;

	FTimerManager& TimerManager = World->GetTimerManager();

	if (TimerManager.TimerExists(StrollFearTimerHandle))
	{
		TimerManager.ClearTimer(StrollFearTimerHandle);
	}
}

void UTPP_BTService_UpdateStrollOfFear::StartStrollFearTimer()
{
	if (!CachedPongo.IsValid())
		return;

	UWorld* World = CachedPongo->GetWorld();
	if (!World)
		return;

	FTimerManager& TimerManager = World->GetTimerManager();

	if (TimerManager.TimerExists(StrollFearTimerHandle) || TimerManager.IsTimerActive(StrollFearTimerHandle))
		return;
	
	TimerManager.SetTimer(
		StrollFearTimerHandle,
		this,
		&UTPP_BTService_UpdateStrollOfFear::OnStrollFearTimerElapsed,
		DelayBeforeCanStroll,
		false
	);
}

void UTPP_BTService_UpdateStrollOfFear::OnStrollFearTimerElapsed()
{
	const bool bHasNearbySafePoint = UpdateNearbySafePoint();
	if (!bHasNearbySafePoint)
	{
		SetBlackboardBool(CanStrollFearKey, false);
		return;
	}

	if (GetBlackboardBool(IsMovingKey))
		return;

	AActor* TargetActor = GetBlackboardActor(TargetActorKey);
	if (!IsValid(TargetActor))
	{
		SetBlackboardBool(CanStrollFearKey, false);
		return;
	}
	if (!GetBlackboardBool(CanStrollFearKey))
		SetBlackboardBool(CanStrollFearKey, true);
}

bool UTPP_BTService_UpdateStrollOfFear::UpdateNearbySafePoint() const
{
	if (!CachedPongo.IsValid())
		return false;

	if (!SafePointClass)
	{
		SetBlackboardBool(HasNearbySafePointKey, false);
		return false;
	}

	UWorld* World = CachedPongo->GetWorld();
	if (!World)
	{
		SetBlackboardBool(HasNearbySafePointKey, false);
		return false;
	}

	const FVector Origin = CachedPongo->GetActorLocation();
	const float Radius = CachedBlackboard->GetValueAsFloat(SafePointSearchRadiusKey.SelectedKeyName);
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(SafePointCollisionChannel);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(CachedPongo.Get());

	TArray<FOverlapResult> Overlaps;
	const bool bHasOverlap = World->OverlapMultiByObjectType(
		Overlaps,
		Origin,
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(Radius),
		QueryParams
	);

	DrawDebugSphere(World, Origin, Radius, 12, bHasOverlap ? FColor::Green : FColor::Red, false, 0.1f);
	
	bool bFoundSafePoint = false;

	if (bHasOverlap)
	{
		for (const FOverlapResult& Result : Overlaps)
		{
			AActor* HitActor = Result.GetActor();
			if (!IsValid(HitActor))
			{
				continue;
			}

			if (HitActor == CachedPongo.Get())
			{
				continue;
			}

			if (!HitActor->IsA(SafePointClass))
			{
				continue;
			}

			bFoundSafePoint = true;
			break;
		}
	}

	SetBlackboardBool(HasNearbySafePointKey, bFoundSafePoint);
	return bFoundSafePoint;
}

void UTPP_BTService_UpdateStrollOfFear::SetBlackboardBool(const FBlackboardKeySelector& Key, bool bValue) const
{
	if (!CachedBlackboard.IsValid())
	{
		return;
	}
	CachedBlackboard->SetValueAsBool(Key.SelectedKeyName, bValue);
}

bool UTPP_BTService_UpdateStrollOfFear::GetBlackboardBool(const FBlackboardKeySelector& Key) const
{
	if (!CachedBlackboard.IsValid())
	{
		return false;
	}

	return CachedBlackboard->GetValueAsBool(Key.SelectedKeyName);
}

AActor* UTPP_BTService_UpdateStrollOfFear::GetBlackboardActor(const FBlackboardKeySelector& Key) const
{
	if (!CachedBlackboard.IsValid())
	{
		return nullptr;
	}
	return Cast<AActor>(CachedBlackboard->GetValueAsObject(Key.SelectedKeyName));
}
