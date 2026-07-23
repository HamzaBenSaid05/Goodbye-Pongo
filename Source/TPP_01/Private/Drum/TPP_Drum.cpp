#include "Drum/TPP_Drum.h"

#include "NavigationSystem.h"
#include "Components/AudioComponent.h"
#include "Drum/TPP_GoThereMarker.h"
#include "EventProxies/TPP_AICommsProxySubsystem.h"
#include "EventProxies/AICommsEvents/Drum/TPP_AIComms_ComeHereEvent.h"
#include "EventProxies/AICommsEvents/Drum/TPP_AIComms_MoveEvent.h"
#include "EventProxies/AICommsEvents/Drum/TPP_AIComms_GoThereEvent.h"
#include "EventProxies/AICommsEvents/Drum/TPP_AIComms_InteractEvent.h"
#include "EventProxies/AICommsEvents/Drum/TPP_AIComms_StopEvent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/TPP_SoundComponent.h"
#include "Drum/TPP_DrumEnums.h"
#include "Gameplay/GameInstances/TPP_GameInstance.h"
#include "VFX/PongoInteractables/TPP_InteractableHighlightingSubsystem.h"

ATPP_Drum::ATPP_Drum()
{
	auto RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);
	SoundComponent = CreateDefaultSubobject<UTPP_SoundComponent>(TEXT("SoundComponent"));

	PlayingVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Playing VFX"));
	PlayingVFX->SetupAttachment(GetRootComponent());
	PlayingVFX->SetAutoActivate(false);

	CommandVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Command VFX"));
	CommandVFX->SetupAttachment(GetRootComponent());
	CommandVFX->SetAutoActivate(false);

	TrailVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trail VFX"));
	TrailVFX->SetupAttachment(GetRootComponent());
	TrailVFX->SetVisibility(false);

	PylonsVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Pylons VFX"));
	PylonsVFX->SetupAttachment(GetRootComponent());

	DrumSphere = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DrumSphere"));
	DrumSphere->SetupAttachment(GetRootComponent());
	DrumPylons = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DrumPylons"));
	DrumPylons->SetupAttachment(GetRootComponent());
	DrumTop = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DrumTop"));
	DrumTop->SetupAttachment(GetRootComponent());

	GoThereConeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GoThereConeMesh"));
	GoThereConeMesh->SetupAttachment(DrumSphere);
	GoThereConeMesh->SetVisibility(false);

	GoThereVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GoThereVFX"));
	GoThereVFX->SetupAttachment(GoThereConeMesh);
	GoThereVFX->SetVisibility(false);

	GoThereLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("GoThereLight"));
	GoThereLight->SetupAttachment(GoThereConeMesh);
	GoThereLight->SetVisibility(false);
}

void ATPP_Drum::ComeHere(const FVector& Position)
{
	if (bGoThereInUse)
	{
		return;
	}
	bool isValid = IsPointOnNavMesh(Position, FVector{50.0f, 50.0f, 100.0f});

	if (!isValid)
		return;
	LastValidPositionComeHere = Position;
}

void ATPP_Drum::ComeHereCompleted()
{
	UTPP_AIComms_ComeHereEvent* ComeHereEvent = NewObject<UTPP_AIComms_ComeHereEvent>();
	ComeHereEvent->Init(LastValidPositionComeHere);
	GetWorld()->GetSubsystem<UTPP_AICommsProxySubsystem>()->NotifyAICommEvent(ComeHereEvent);

	CommandVFX->SetVariableInt("FrameToUse", static_cast<int32>(ETPP_DrumCommands::ComeHere));
	PlayDrumWave();

	//====Sound====
	if (FMath::RandRange(1, 5) <= JaneTrustCall)
	{
		FTPP_AudioParameter Parameter = FTPP_AudioParameter();
		Parameter.Int = 0;
		SoundComponent->PlaySound(TagAction, GetActorLocation(), AudioCompComeHere, Parameter);
	}
	else
	{
		FTPP_AudioParameter Parameter = FTPP_AudioParameter();
		Parameter.Int = 0;
		SoundComponent->PlaySound(TagAction, GetActorLocation(), AudioCompComeHere, Parameter);
	}
	bIsValidPosition = false;
}

void ATPP_Drum::Stop()
{
	UTPP_AIComms_StopEvent* StopEvent = NewObject<UTPP_AIComms_StopEvent>();
	GetWorld()->GetSubsystem<UTPP_AICommsProxySubsystem>()->NotifyAICommEvent(StopEvent);

	CommandVFX->SetVariableInt("FrameToUse", static_cast<int32>(ETPP_DrumCommands::Stop));
	PlayDrumWave();
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = 1;
	//====Sound====
	SoundComponent->PlaySound(TagAction, GetActorLocation(), AudioCompStop, Parameter);
}

void ATPP_Drum::GoThereStarted_Implementation()
{
	Marker->SetActorLocation(GetActorLocation());
	bGoThereInUse = true;
}

void ATPP_Drum::GoThere_Implementation(const FVector& MarkerDirection, const FVector& StartPositionMarker, const float& DrumHeight)
{
	if (!bGoThereInUse)
	{
		return;
	}

	FVector CheckValidPosition = Marker->ActiveMarker(MarkerDirection, StartPositionMarker, MaxDistance, DrumHeight);
	CheckValidPosition.Z = CheckValidPosition.Z - DrumHeight;
	IsPointOnNavMesh(CheckValidPosition, FVector{50.0f, 50.0f, 50.0f});
}

void ATPP_Drum::GoThereCompleted_Implementation()
{
	if (bIsValidPosition)
	{
		UTPP_AIComms_GoThereEvent* GoThereEvent = NewObject<UTPP_AIComms_GoThereEvent>();
		GoThereEvent->Init(LastValidPositionGoThere);

		CommandVFX->SetVariableInt("FrameToUse", static_cast<int32>(ETPP_DrumCommands::GoThere));
		PlayDrumWave();
		FTPP_AudioParameter Parameter = FTPP_AudioParameter();
		Parameter.Int = 5;
		GetWorld()->GetSubsystem<UTPP_AICommsProxySubsystem>()->NotifyAICommEvent(GoThereEvent);
		SoundComponent->PlaySound(TagAction, GetActorLocation(), AudioCompGoThereCompleted, Parameter);
	}

	DeactivateMarker();
	DisableGoThereSound();
}

void ATPP_Drum::GoThereCancelled_Implementation()
{
	OnCancelledFinished.Broadcast(false);
	bGoThereInUse = false;
	DisableGoThereSound();
	DeactivateMarker();
}

void ATPP_Drum::Interact()
{
	if (!bInteractActive)
		return;

	const UTPP_AIComms_InteractEvent* InteractEvent = NewObject<UTPP_AIComms_InteractEvent>();
	bIsValidPosition = false;
	GetWorld()->GetSubsystem<UTPP_AICommsProxySubsystem>()->NotifyAICommEvent(InteractEvent);

	if (auto InteractableHighlightingSubsystem = GetWorld()->GetSubsystem<UTPP_InteractableHighlightingSubsystem>())
		InteractableHighlightingSubsystem->PlayInteractionVFX();

	CommandVFX->SetVariableInt("FrameToUse", static_cast<int32>(ETPP_DrumCommands::Interact));
	PlayDrumWave();

	//====Sound====
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = 2;
	SoundComponent->PlaySound(TagAction, GetActorLocation(), AudioCompInteract, Parameter);
}

bool ATPP_Drum::IsPointOnNavMesh(const FVector& Location, const FVector& Extends)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys)
		return false;

	FNavLocation ProjectedLocation;

	NavSys->ProjectPointToNavigation(Location, ProjectedLocation, Extends);

	float Distance = FVector::Dist(Location, ProjectedLocation.Location);

	if (Distance < GoThereDistanceNavMesh)
	{
		if (bGoThereInUse)
		{
			ConeMeshMaterialInst->SetVectorParameterValue("ConeColor", Marker->ValidColor);
			GoThereVFX->SetColorParameter(FName("InnerColor"), Marker->ValidColor);
			GoThereLight->SetLightColor(Marker->ValidColor);
			Marker->SetColor(Marker->ValidColor);
			LastValidPositionGoThere = ProjectedLocation.Location;
		}
		bIsValidPosition = true;
		return true;
	}

	if (bGoThereInUse)
	{
		GoThereLight->SetLightColor(Marker->InvalidColor);
		GoThereVFX->SetColorParameter(FName("InnerColor"), Marker->InvalidColor);
		ConeMeshMaterialInst->SetVectorParameterValue("ConeColor", Marker->InvalidColor);
		Marker->SetColor(Marker->InvalidColor);
	}
	return false;
}

void ATPP_Drum::EnableGoThere(const bool bShouldActivate)
{
	bGoThereActive = bShouldActivate;

	if (const UTPP_GameInstance* GI = Cast<UTPP_GameInstance>(GetGameInstance()))
	{
		UTPP_SaveGame* SaveGameObject = GI->GetSaveGameObject();

		SaveGameObject->bGoThereActive = bGoThereActive;

		UGameplayStatics::SaveGameToSlot
		(
			SaveGameObject,
			SaveGameObject->SaveSlotName,
			SaveGameObject->UserIndex
		);
	}
}

void ATPP_Drum::EnableInteract(const bool bShouldActivate)
{
	bInteractActive = bShouldActivate;

	if (const UTPP_GameInstance* GI = Cast<UTPP_GameInstance>(GetGameInstance()))
	{
		UTPP_SaveGame* SaveGameObject = GI->GetSaveGameObject();

		SaveGameObject->bInteractActive = bInteractActive;

		UGameplayStatics::SaveGameToSlot
		(
			SaveGameObject,
			SaveGameObject->SaveSlotName,
			SaveGameObject->UserIndex
		);
	}
}

void ATPP_Drum::ActiveGoThereStartedSound()
{
	if (AudioCompGoThere != nullptr) return;
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = 3;
	SoundComponent->PlaySound(TagAction, GetActorLocation(), AudioCompGoThere, Parameter);
}

void ATPP_Drum::ActiveGoThereSound()
{
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = 4;
	SoundComponent->PlaySound(TagAction, GetActorLocation(), AudioCompGoThere, Parameter);
}

void ATPP_Drum::DisableGoThereSound()
{
	if (AudioCompGoThere != nullptr)
	{
		AudioCompGoThere->Stop();
		AudioCompGoThere = nullptr;
	}
}

void ATPP_Drum::DeactivateMarker()
{
	Marker->DisableMarker();
	LastValidPositionGoThere = FVector::ZeroVector;
	bIsValidPosition = false;
}

void ATPP_Drum::ToggleTrail(bool bShouldActivate) { TrailVFX->SetVisibility(bShouldActivate); }

void ATPP_Drum::PlayDrumWave()
{
	PlayingVFX->ActivateSystem();
	CommandVFX->ActivateSystem();
}

void ATPP_Drum::TogglePylonRotation(bool bShouldActivate)
{
	float NewRate = bShouldActivate ? ActivePylonRotationRate : 0.f;
	PylonsVFX->SetFloatParameter("RotationRatio", NewRate);
}

void ATPP_Drum::PlayAcquisitionAnimation()
{
	SetActorHiddenInGame(false);

	if (DrumPylons && PylonAcquisitionMontage) { DrumPylons->PlayAnimation(PylonAcquisitionMontage.Get(), false); }

	if (DrumSphere && SphereAcquisitionMontage) { DrumSphere->PlayAnimation(SphereAcquisitionMontage.Get(), false); }

	if (DrumTop && TopAcquisitionMontage) { DrumTop->PlayAnimation(TopAcquisitionMontage.Get(), false); }
}

void ATPP_Drum::HandleDrumAcquired()
{
	OnDrumAcquired.Broadcast(GetOwner());
}

void ATPP_Drum::BeginPlay()
{
	Super::BeginPlay();

	if (const UTPP_GameInstance* GI = Cast<UTPP_GameInstance>(GetGameInstance()))
	{
		const UTPP_SaveGame* SaveGameObject = GI->GetSaveGameObject();
		EnableGoThere(SaveGameObject->bGoThereActive);
		EnableInteract(SaveGameObject->bInteractActive);
	}

	MaterialInstances.Add(DrumSphere->CreateDynamicMaterialInstance(0));
	MaterialInstances.Add(DrumPylons->CreateDynamicMaterialInstance(0));
	MaterialInstances.Add(DrumTop->CreateDynamicMaterialInstance(0));
	MaterialInstances.Add(DrumTop->CreateDynamicMaterialInstance(1));

	ConeMeshMaterialInst = GoThereConeMesh->CreateDynamicMaterialInstance(0);
}
