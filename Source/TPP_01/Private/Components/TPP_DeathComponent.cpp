#include "Components/TPP_DeathComponent.h"

UTPP_DeathComponent::UTPP_DeathComponent()
{
	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimeline"));
	bIsDying = false;
}

void UTPP_DeathComponent::Killed_Implementation(const FDeathInfo& DeathInfo)
{
	SavedDeathInfo = DeathInfo;

	switch (DeathInfo.DeathCause)
	{
	case EDeathCause::Statue:
		{
			Dissolve(DeathInfo.Mesh);
			break;
		}
	case EDeathCause::KillBox:
		{
			FinishDeath();
			UE_LOG(LogTemp, Display, TEXT("Killed by KillBox"));
			break;
		}
	case EDeathCause::Pillar:
		{
			// TODO: play eventual vfx/anim
			FinishDeath();
			UE_LOG(LogTemp, Display, TEXT("Killed by Pillar"));
			break;
		}
	default:
		{
			UE_LOG(LogTemp, Display, TEXT("no death cause received"));
			break;
		}
	}
}

void UTPP_DeathComponent::BeginPlay()
{
	Super::BeginPlay();

	DissolveTimelineProgress.BindDynamic(this, &UTPP_DeathComponent::DissolveProgress);
	DissolveTimelineFinishedEvent.BindDynamic(this, &UTPP_DeathComponent::FinishDeath);

	if (DissolveCurve)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, MoveTemp(DissolveTimelineProgress));
		DissolveTimeline->SetTimelineFinishedFunc(MoveTemp(DissolveTimelineFinishedEvent));
	}
}

void UTPP_DeathComponent::Dissolve(class USkeletalMeshComponent* Mesh)
{
	TArray<UMeshComponent*> StaticMeshComps;
	GetOwner()->GetComponents(UMeshComponent::StaticClass(), StaticMeshComps);

	for (UMeshComponent* SM : StaticMeshComps)
	{
		for (int i = 0; i < SM->GetNumMaterials(); ++i)
		{
			DynamicMaterialInst.Add(SM->CreateDynamicMaterialInstance(i));
		}
	}

	DissolveTimeline->PlayFromStart();
}

void UTPP_DeathComponent::DissolveProgress(const float Output)
{
	for (UMaterialInstanceDynamic* MaterialInst : DynamicMaterialInst)
	{
		MaterialInst->SetScalarParameterValue(FName("DissolveAmount"), Output);
	}
}

void UTPP_DeathComponent::FinishDeath()
{
	OnDead.Broadcast(SavedDeathInfo);
}
