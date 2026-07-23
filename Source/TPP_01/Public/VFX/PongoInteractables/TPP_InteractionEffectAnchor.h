#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPP_InteractionEffectAnchor.generated.h"

UCLASS()
class TPP_01_API ATPP_InteractionEffectAnchor : public AActor
{
	GENERATED_BODY()

public:
	ATPP_InteractionEffectAnchor();

	UFUNCTION(BlueprintPure)
	class UNiagaraComponent* GetPlayingVFX() const { return PlayingVFX; }
	
	UFUNCTION(BlueprintPure)
	class UNiagaraComponent* GetTrailVFX() const { return TrailVFX; }

	void PlayVFX(float InSize = 1.0f);
	
	void ToggleTrail(const bool bActivate);

private:
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* PlayingVFX;
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* TrailVFX;
};
