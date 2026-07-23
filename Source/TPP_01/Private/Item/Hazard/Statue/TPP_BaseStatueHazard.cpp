#include "Item/Hazard/Statue/TPP_BaseStatueHazard.h"

#include "NiagaraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TPP_SoundComponent.h"
#include "Gameplay/GameModes/TPP_GameMode.h"
#include "Interfaces/Killable.h"
#include "Kismet/GameplayStatics.h"

ATPP_BaseStatueHazard::ATPP_BaseStatueHazard()
{
	HazardMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HazardMesh"));
	SetRootComponent(HazardMesh);
	
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("LightComp"));
	SpotLight->SetupAttachment(GetRootComponent());
	SpotLight->SetVisibility(false);

	LightCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpotLight"));
	LightCone->SetupAttachment(GetRootComponent());
	LightCone->SetAutoActivate(true);
	
	StaticVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Static VFX"));
	StaticVFX->SetupAttachment(GetRootComponent());
	StaticVFX->SetAutoActivate(false);
	
	PrimaryActorTick.bStartWithTickEnabled = true;
	Emitter = CreateDefaultSubobject<USceneComponent>(TEXT("Emitter"));
	
	bShouldSoftReset = false;
}

void ATPP_BaseStatueHazard::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle SpawningHandle;
	GetWorldTimerManager().SetTimer(
		SpawningHandle,
		this,
		&ThisClass::InitTargets,
		3,
		false
	);

	LightCone->OnComponentBeginOverlap.AddDynamic(this, &ATPP_BaseStatueHazard::KillTargets);

	LightConeMI = LightCone->CreateDynamicMaterialInstance(0);
	LightConeMI->SetVectorParameterValue(FName("ConeColor"), IdleColor);
	StaticVFX->SetColorParameter(FName("InnerColor"), IdleColor);
	SpotLight->SetLightColor(IdleColor);

	InitialLightConeScale = LightCone->GetComponentScale();
	LightCone->SetRelativeScale3D(FVector::ZeroVector);
}

void ATPP_BaseStatueHazard::InitTargets()
{
	if (const ATPP_GameMode* GM = Cast<ATPP_GameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GM->GetCharacters(Targets);
	}
}

void ATPP_BaseStatueHazard::KillTargets(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                        bool bFromSweep, const FHitResult& SweepResult)
{
	LightConeMI->SetVectorParameterValue(FName("ConeColor"), SpottedColor);
	StaticVFX->SetColorParameter(FName("InnerColor"), SpottedColor);
	SpotLight->SetLightColor(SpottedColor);
	
	if (OtherActor)
	{
		if (AudioCompKill == nullptr)
		{
			FTPP_AudioParameter Parameter = FTPP_AudioParameter();
			Parameter.Int = 2;
			FVector Position = Emitter->GetRelativeLocation() + GetActorLocation();
			SoundComponent->PlaySound(TagStatue,Position,AudioCompKill,Parameter);
		}
		if (OtherActor->ActorHasTag("Player"))
		{
			IKillable::Execute_Kill(OtherActor, bShouldSoftReset,ECharacterDeath::PlayerDeath,DeathCause);
		}
		else if (OtherActor->ActorHasTag("Pongo"))
		{
			IKillable::Execute_Kill(OtherActor, bShouldSoftReset,ECharacterDeath::PongoDeath,DeathCause);
		}
	}
}

void ATPP_BaseStatueHazard::InitializeStatueAnim()
{
	UAnimInstance* AnimInst = HazardMesh->GetAnimInstance();

	HazardMesh->SetPlayRate(1);
	AnimInst->StopAllMontages(0.f);
	AnimInst->Montage_Play(ActivationAnim, 1.f);
	AnimInst->Montage_JumpToSection(FName("Default"), ActivationAnim);
}

void ATPP_BaseStatueHazard::DeinitializeStatueAnim()
{
	HazardMesh->SetPosition(DeinitPosition);
	HazardMesh->InitAnim(true);
	HazardMesh->SetPlayRate(-1);
	HazardMesh->GetAnimInstance()->Montage_SetNextSection(FName("LoopStart"), FName("Default"));
}

void ATPP_BaseStatueHazard::ToggleStatueActive(const bool bShouldBeActive)
{
	ToggleLightCone(bShouldBeActive);
	StaticVFX->SetActive(bShouldBeActive, true);
	SpotLight->SetVisibility(bShouldBeActive);
}
