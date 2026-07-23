#include "RoomManagement/TPP_RoomVolume.h"

#include "Components/BoxComponent.h"
#include "EventProxies/TPP_RoomsProxySubsystem.h"
#include "EventProxies/RoomsEvents/TPP_EnteredRoomEvent.h"
#include "EventProxies/RoomsEvents/TPP_ExitRoomEvent.h"
#include "EventProxies/RoomsEvents/TPP_PongoChangedRoomEvent.h"

ATPP_RoomVolume::ATPP_RoomVolume()
{
	auto Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("RoomVolume"));
	Volume->SetupAttachment(Root);
	Volume->SetCollisionResponseToAllChannels(ECR_Ignore);
	Volume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ATPP_RoomVolume::BeginPlay()
{
	Super::BeginPlay();

	Volume->OnComponentBeginOverlap.AddDynamic(this, &ATPP_RoomVolume::PlayerEnteredRoom);
	Volume->OnComponentEndOverlap.AddDynamic(this, &ATPP_RoomVolume::PlayerExitRoom);
}

void ATPP_RoomVolume::PlayerEnteredRoom(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                        const FHitResult& SweepResult)
{
	if (UTPP_RoomsProxySubsystem* RoomSubsystem = GetWorld()->GetSubsystem<UTPP_RoomsProxySubsystem>())
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			UTPP_EnteredRoomEvent* NewEvent = NewObject<UTPP_EnteredRoomEvent>();
			NewEvent->Init(this);

			RoomSubsystem->NotifyRoomEvent(NewEvent);
		}
		else if (OtherActor->ActorHasTag("Pongo"))
		{
			UTPP_PongoChangedRoomEvent* NewEvent = NewObject<UTPP_PongoChangedRoomEvent>();
			NewEvent->Init(this);

			RoomSubsystem->NotifyRoomEvent(NewEvent);
		}
	}
}

void ATPP_RoomVolume::PlayerExitRoom(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (UTPP_RoomsProxySubsystem* RoomSubsystem = GetWorld()->GetSubsystem<UTPP_RoomsProxySubsystem>())
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			UTPP_ExitRoomEvent* NewEvent = NewObject<UTPP_ExitRoomEvent>();
			NewEvent->Init(this);

			RoomSubsystem->NotifyRoomEvent(NewEvent);
		}
	}
}

void ATPP_RoomVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (EdgePoints.Num() < 2) return;

	const FVector TransformedRoomStart = GetActorTransform().TransformPosition(EdgePoints[0]);
	const FVector TransformedRoomEnd = GetActorTransform().TransformPosition(EdgePoints[1]);

	const FVector RoomVector = TransformedRoomEnd - TransformedRoomStart;

	Volume->SetWorldLocation(TransformedRoomStart + RoomVector * 0.5f);

	Volume->SetBoxExtent(
	                     FVector(
	                             RoomVector.X * 0.5f,
	                             Volume->GetScaledBoxExtent().Y,
	                             Volume->GetScaledBoxExtent().Z
	                            ),
	                     true
	                    );
}
