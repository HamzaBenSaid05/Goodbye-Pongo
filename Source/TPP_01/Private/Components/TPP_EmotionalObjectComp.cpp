#include "Components/TPP_EmotionalObjectComp.h"
#include "Characters/Pongo/TPP_Pongo.h"
#include "EventProxies/TPP_AICommsProxySubsystem.h"
#include "EventProxies/AICommsEvents/EmotionalState/TPP_AIComms_ChangeEmotionalStateEvent.h"
#include "EventProxies/AICommsEvents/EmotionalState/TPP_AIComms_ResetEvent.h"
#include "DrawDebugHelpers.h"
#include "Item/Interactable/TPP_InteractableItem.h"
#include "Kismet/GameplayStatics.h"

UTPP_EmotionalObjectComp::UTPP_EmotionalObjectComp()
{
	PrimaryComponentTick.bCanEverTick = false;
	PongoChannel = ECC_GameTraceChannel1;
}

void UTPP_EmotionalObjectComp::SendEmotionalStateEvent(ETPP_PongoState NewEmotionalState)
{
	if (UTPP_AICommsProxySubsystem* Sub = GetWorld()->GetSubsystem<UTPP_AICommsProxySubsystem>())
	{
		UTPP_AIComms_ChangeEmotionalStateEvent* Event = NewObject<UTPP_AIComms_ChangeEmotionalStateEvent>();
		Event->Init(NewEmotionalState, GetOwner());
		UE_LOG(LogTemp, Log, TEXT("EmotionalState : %i"), NewEmotionalState);
		Sub->NotifyAICommEvent(Event);
	}
}

void UTPP_EmotionalObjectComp::ResetEmotionalState()
{
	if (UTPP_AICommsProxySubsystem* Sub = GetWorld()->GetSubsystem<UTPP_AICommsProxySubsystem>())
	{
		UTPP_AIComms_ResetEvent* ResetState = NewObject<UTPP_AIComms_ResetEvent>();
		UE_LOG(LogTemp, Log, TEXT("Reset State"));
		Sub->NotifyAICommEvent(ResetState);
	}
}

void UTPP_EmotionalObjectComp::BeginPlay()
{
	Super::BeginPlay();
	UAudioComponent* AudioComp = nullptr;
	ATPP_InteractableItem* Item = Cast<ATPP_InteractableItem>(GetOwner());
	if (Item != nullptr)
		Item->SoundComponent->PlaySound(TagEmotionalState,GetRelativeLocation(),AudioComp);
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(PongoChannel, ECR_Overlap);
	SetHiddenInGame(true);
	OnComponentBeginOverlap.AddDynamic(this, &UTPP_EmotionalObjectComp::OnOverlapBegin);
	OnComponentEndOverlap.AddDynamic(this, &UTPP_EmotionalObjectComp::OnOverlapEnd);
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void UTPP_EmotionalObjectComp::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner() || !OtherActor->ActorHasTag("Pongo"))
		return;

	PongoActor = Cast<ATPP_Pongo>(OtherActor);
	bEventSent = false;

	// Start check timer, it stops by itself when the event is sent
	GetWorld()->GetTimerManager().SetTimer(PongoTimerHandle, [this]() { CheckPongoVisibility(); }, 0.1f, true);
}

void UTPP_EmotionalObjectComp::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == GetOwner() || !OtherActor->ActorHasTag("Pongo"))
		return;

	GetWorld()->GetTimerManager().ClearTimer(PongoTimerHandle);
	bEventSent = false;
	PongoActor = nullptr;

	if (OtherActor->GetVelocity() != FVector::ZeroVector)
		return;

	UE_LOG(LogTemp, Log, TEXT("Overlap Ended with %s"), *OtherActor->GetName());
	if (bAutoDestroy)
		return;
	ResetEmotionalState();
}

void UTPP_EmotionalObjectComp::CheckPongoVisibility()
{
	ATPP_Pongo* Pongo = PongoActor.Get();
	if (!Pongo)
	{
		GetWorld()->GetTimerManager().ClearTimer(PongoTimerHandle);
		return;
	}

	// Event sent, timer stops
	if (bEventSent)
	{
		GetWorld()->GetTimerManager().ClearTimer(PongoTimerHandle);
		return;
	}

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.AddIgnoredActor(Pongo);
	Params.AddIgnoredActor(PlayerPawn.Get());
	if (AActor* Interactable = Pongo->GetInteractableItem())
		if (IsValid(Interactable))
			Params.AddIgnoredActor(Interactable);

	FVector Start = GetOwner()->GetActorLocation();
	FVector End = Pongo->GetMesh()->GetBoneLocation(HeadBoneName);

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.15f, 0, 2.f);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		// Obstacle, try again at next timer
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10, 12, FColor::Red, false, 0.15f);
		// Print obstacle name
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("Obstacle: %s"), *Hit.GetActor()->GetName()));
		return;
	}

	// Visible -> send event and stop timer
	bEventSent = true;
	GetWorld()->GetTimerManager().ClearTimer(PongoTimerHandle);
	SendEmotionalStateEvent(EmotionalState);
}

void UTPP_EmotionalObjectComp::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(PongoTimerHandle);
}

void UTPP_EmotionalObjectComp::OnRegister()
{
	Super::OnRegister();
	SetGenerateOverlapEvents(true);
	SetHiddenInGame(true);
	SetCanEverAffectNavigation(false);
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionObjectType(ECC_WorldDynamic);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(PongoChannel, ECR_Overlap);
}
