#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "TPP_NotifyDrumAcquisition.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_NotifyDrumAcquisition : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
