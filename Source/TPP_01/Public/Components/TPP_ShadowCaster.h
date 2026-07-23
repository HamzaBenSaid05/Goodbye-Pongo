#pragma once

#include "CoreMinimal.h"
#include "Components/DecalComponent.h"
#include "TPP_ShadowCaster.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPP_01_API UTPP_ShadowCaster : public UDecalComponent
{
	GENERATED_BODY()

	UTPP_ShadowCaster();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditDefaultsOnly)
	float TraceLength;
	
	UPROPERTY(EditDefaultsOnly)
	float ScaleMultiplier;

	float StartingSize;
	float LengthMultiplier;
	
	void OnTraceCompleted(const FTraceHandle& TraceHandle, FTraceDatum& TraceDatum);
	FTraceDelegate TraceDelegate;
};
