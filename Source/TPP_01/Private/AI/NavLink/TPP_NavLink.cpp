#include "AI/NavLink/TPP_NavLink.h"

#include "NavLinkCustomComponent.h"
#include "Interfaces/AI/TPP_AIAction.h"

ATPP_NavLink::ATPP_NavLink()
{
	PrimaryActorTick.bCanEverTick = true;
	bSmartLinkIsRelevant = true;
	PointLinks.Empty();
}

void ATPP_NavLink::BeginPlay()
{
	Super::BeginPlay();
	OnSmartLinkReached.AddDynamic(
	                              this, &ATPP_NavLink::NotifyJumpLink);
}

#if WITH_EDITOR
void ATPP_NavLink::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!GetWorld() || !GetSmartLinkComp())
	{
		return;
	}
	
	FVector End = GetSmartLinkComp()->GetEndPoint();

	End.Z += DestinationZOffset;

	// World -> Local
	JumpDestination =
		GetActorTransform().InverseTransformPosition(End);
}
#endif

void ATPP_NavLink::NotifyJumpLink(AActor* PathingAgent, const FVector& DestPoint)
{
	if (PathingAgent)
	{
		if (ITPP_AIAction* JumpableAgent = Cast<ITPP_AIAction>(PathingAgent))
		{
			JumpableAgent->Jump(DestPoint, DestinationZOffset);
#if WITH_EDITOR
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Jump");
				FVector End = FVector(DestPoint.X, DestPoint.Y, DestPoint.Z + DestinationZOffset);
				DrawDebugSphere(GetWorld(), End, 50.f, 12, FColor::Red, false, 5.f);
			}
#endif
		}
	}
}
