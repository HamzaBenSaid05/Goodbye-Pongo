#include "Animation/Notify/TPP_NotifyPetFinished.h"

#include "Interfaces/AI/TPP_AIPettable.h"

void UTPP_NotifyPetFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	
	if (Owner->GetInstigatorController()->Implements<UTPP_AIPettable>())
	{
		ITPP_AIPettable::Execute_PetFinished(Owner->GetInstigatorController());
	}
}
