#include "AI/Tasks/Decorator/TPP_BTDecorator_IsNotOnNavMesh.h"

#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

UTPP_BTDecorator_IsNotOnNavMesh::UTPP_BTDecorator_IsNotOnNavMesh() { NodeName = TEXT("Is Not On NavMesh"); }

bool UTPP_BTDecorator_IsNotOnNavMesh::CalculateRawConditionValue(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory
) const
{
	if (!Super::CalculateRawConditionValue(OwnerComp, NodeMemory)) { return false; }

	UNavigationSystemV1* NavSys =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(
		                                                   CachedPawn->GetWorld());

	if (!NavSys)
	{
		UE_LOG(LogTemp, Warning, TEXT("No NavigationSystem found"));
		return true;
	}

	FVector TestLocation = CachedPawn->GetActorLocation();

	// Se è un Character, controlliamo i piedi invece del centro capsule
	if (const ACharacter* Character = Cast<ACharacter>(CachedPawn))
	{
		if (const UCapsuleComponent* Capsule = Character->GetCapsuleComponent()) { TestLocation.Z -= Capsule->GetScaledCapsuleHalfHeight(); }
	}

	const ANavigationData* NavData =
		NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);

	FNavLocation NavLocation;

	const bool bOnNavMesh = NavSys->ProjectPointToNavigation(
	                                                         TestLocation,
	                                                         NavLocation,
	                                                         FVector(100.f, 100.f, 300.f),
	                                                         NavData
	                                                        );

	const float Dist =
		bOnNavMesh
			? FVector::Dist(TestLocation, NavLocation.Location)
			: -1.f;

	UE_LOG(
	       LogTemp,
	       Warning,
	       TEXT("Actor=%s | Test=%s | OnNavMesh=%d | Dist=%.2f"),
	       *CachedPawn->GetActorLocation().ToString(),
	       *TestLocation.ToString(),
	       bOnNavMesh,
	       Dist
	      );
	if (Dist < 0.f)
	{
		return false;
	}
#if WITH_EDITOR
	DrawDebugSphere(
	                CachedPawn->GetWorld(),
	                TestLocation,
	                20.f,
	                12,
	                bOnNavMesh ? FColor::Green : FColor::Red,
	                false,
	                1.f
	               );

	if (bOnNavMesh)
	{
		DrawDebugSphere(
		                CachedPawn->GetWorld(),
		                NavLocation.Location,
		                20.f,
		                12,
		                FColor::Blue,
		                false,
		                1.f
		               );

		DrawDebugLine(
		              CachedPawn->GetWorld(),
		              TestLocation,
		              NavLocation.Location,
		              FColor::Yellow,
		              false,
		              1.f,
		              0,
		              2.f
		             );
	}
#endif


	if (!bOnNavMesh) { return true; }

	return Dist > 15.f;
}
