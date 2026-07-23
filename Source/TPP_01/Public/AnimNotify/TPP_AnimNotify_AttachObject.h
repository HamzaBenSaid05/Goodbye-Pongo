#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "TPP_AnimNotify_AttachObject.generated.h"

UCLASS()
class TPP_01_API UTPP_AnimNotify_AttachObject : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual auto Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) -> void override;

	UPROPERTY(EditAnywhere, Category = "Notify")
	FName SocketHandName = "ItemPickUp";

	UPROPERTY(EditAnywhere, Category = "Notify")
	FName EmotionalObjectKeyName = NAME_None;
};
