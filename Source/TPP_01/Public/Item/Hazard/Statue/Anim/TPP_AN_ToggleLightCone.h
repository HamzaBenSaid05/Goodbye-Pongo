#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "TPP_AN_ToggleLightCone.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_AN_ToggleLightCone : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual FString GetNotifyName_Implementation() const override { return TEXT("ToggleLightCone"); }

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
