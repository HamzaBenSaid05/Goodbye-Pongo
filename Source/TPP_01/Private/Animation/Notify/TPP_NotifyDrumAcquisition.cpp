#include "Animation/Notify/TPP_NotifyDrumAcquisition.h"

#include "Drum/TPP_Drum.h"

void UTPP_NotifyDrumAcquisition::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (ATPP_Drum* Drum = Cast<ATPP_Drum>(MeshComp->GetOwner()))
	{
		Drum->HandleDrumAcquired();
	}
}
