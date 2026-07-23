#include "VFX/PongoInteractables/TPP_InteractableHighlightingSubsystem.h"

#include "DeveloperSettings/TPP_DeveloperSettings.h"
#include "Kismet/GameplayStatics.h"
#include "VFX/PongoInteractables/PulseHandler.h"
#include "VFX/PongoInteractables/TPP_CollarHandler.h"
#include "VFX/PongoInteractables/TPP_InteractionEffectAnchor.h"

void UTPP_InteractableHighlightingSubsystem::RegisterActor(UPulseHandler* InComp) { CachedInteractables.AddUnique(InComp); }

void UTPP_InteractableHighlightingSubsystem::UnregisterActor(UPulseHandler* InComp) { CachedInteractables.Remove(InComp); }

void UTPP_InteractableHighlightingSubsystem::StartScanning(float InInterval)
{
	Interval = InInterval;
	
	GetWorld()->GetTimerManager().SetTimer
		(
		 TimerHandle,
		 this,
		 &UTPP_InteractableHighlightingSubsystem::TickTask,
		 InInterval,
		 true
		);
}

void UTPP_InteractableHighlightingSubsystem::StopScanning() { GetWorld()->GetTimerManager().ClearTimer(TimerHandle); }

void UTPP_InteractableHighlightingSubsystem::SetPongoRef(AActor* InPongoRef) { PongoRef = InPongoRef; }

void UTPP_InteractableHighlightingSubsystem::SetMaxDistance(float InMaxDistance) { MaxDistance = InMaxDistance; }

void UTPP_InteractableHighlightingSubsystem::PlayInteractionVFX()
{
	if (!CurrentClosest.IsValid() || !IsValid(InteractionEffectAnchor))
		return;
	
	StopScanning();

	const AActor* ClosestItem = CurrentClosest->GetOwner();
	
	FVector Origin, Extents;
	ClosestItem->GetActorBounds(true, Origin, Extents);
	
	InteractionEffectAnchor->PlayVFX((Extents.X+Extents.Y) * 4);
	
	StartScanning(Interval);
}

void UTPP_InteractableHighlightingSubsystem::Initialize(FSubsystemCollectionBase& Collection) { Super::Initialize(Collection); }

void UTPP_InteractableHighlightingSubsystem::Deinitialize() { Super::Deinitialize(); }

void UTPP_InteractableHighlightingSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	InteractionEffectAnchor = Cast<ATPP_InteractionEffectAnchor>(UGameplayStatics::GetActorOfClass(GetWorld(), ATPP_InteractionEffectAnchor::StaticClass()));
}

bool UTPP_InteractableHighlightingSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
		return false;

	const UTPP_DeveloperSettings* GameplaySettings = GetDefault<UTPP_DeveloperSettings>();
	check(GameplaySettings);

	const UWorld* Level = CastChecked<UWorld>(Outer);

	return GameplaySettings->IsGameplayLevel(Level);
}

void UTPP_InteractableHighlightingSubsystem::TickTask()
{
	if (CachedInteractables.IsEmpty()) return;
	if (!PongoRef.IsValid()) return;

	const int32 NumActors = CachedInteractables.Num();

	float ClosestDistance = FLT_MAX;
	TWeakObjectPtr<UPulseHandler> ClosestItem = nullptr;

	const FVector PongoLoc = PongoRef->GetActorLocation();

	for (const TWeakObjectPtr<UPulseHandler>& CachedInteractable : CachedInteractables)
	{
		const float SqDist = FVector::DistSquared(PongoLoc, CachedInteractable.Get()->GetOwner()->GetActorLocation());

		if (SqDist > MaxDistance * MaxDistance) continue;

		if (FMath::Min(SqDist, ClosestDistance) < ClosestDistance)
		{
			ClosestDistance = SqDist;
			ClosestItem = CachedInteractable;
		}
	}

	if (!ClosestItem.IsValid())
	{
		if (CurrentClosest.IsValid())
		{
			ITPP_CollarHandler::Execute_TogglePulse(PongoRef.Get(), false);
			CurrentClosest->TogglePulse();
			CurrentClosest.Reset();
		}

		return;
	}

	if (CurrentClosest.IsValid())
		CurrentClosest->TogglePulse();

	ITPP_CollarHandler::Execute_TogglePulse(PongoRef.Get(), true);
	CurrentClosest = ClosestItem;
	InteractionEffectAnchor->AttachToActor(CurrentClosest->GetOwner(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("VFXAnchor"));
	
	CurrentClosest->TogglePulse();
}
