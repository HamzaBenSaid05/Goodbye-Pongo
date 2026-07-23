#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TPP_InteractableHighlightingSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_InteractableHighlightingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	void RegisterActor(class UPulseHandler* InComp);
	void UnregisterActor(class UPulseHandler* InComp);
	
	void StartScanning(float InInterval);
	void StopScanning();
	
	void SetPongoRef(AActor* InPongoRef);

	void SetMaxDistance(float InMaxDistance);
	
	void PlayInteractionVFX();
	
	UPROPERTY()
	TObjectPtr<class ATPP_InteractionEffectAnchor> InteractionEffectAnchor;
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
private:
	TWeakObjectPtr<AActor> PongoRef;
	TArray<TWeakObjectPtr<class UPulseHandler>> CachedInteractables;
	TWeakObjectPtr<class UPulseHandler> CurrentClosest;
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
	void TickTask();
	
	FTimerHandle TimerHandle;

	float MaxDistance;
	
	float Interval;
};
