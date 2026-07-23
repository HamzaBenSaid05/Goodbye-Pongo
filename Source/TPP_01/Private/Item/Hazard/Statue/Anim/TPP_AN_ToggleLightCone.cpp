#include "Item/Hazard/Statue/Anim/TPP_AN_ToggleLightCone.h"

#include "Item/Hazard/Statue/TPP_BaseStatueHazard.h"

void UTPP_AN_ToggleLightCone::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (ATPP_BaseStatueHazard* Statue = Cast<ATPP_BaseStatueHazard>(MeshComp->GetOwner()))
	{
		Statue->ToggleStatueActive(true);
	}
}

void UTPP_AN_ToggleLightCone::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (ATPP_BaseStatueHazard* Statue = Cast<ATPP_BaseStatueHazard>(MeshComp->GetOwner()))
	{
		Statue->ToggleStatueActive(false);
	}
}
