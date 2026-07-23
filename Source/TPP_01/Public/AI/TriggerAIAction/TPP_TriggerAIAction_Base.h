#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "TPP_TriggerAIAction_Base.generated.h"

UCLASS()
class TPP_01_API ATPP_TriggerAIAction_Base : public ATriggerBox
{
	GENERATED_BODY()

public:
	ATPP_TriggerAIAction_Base();

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);
	void virtual PongoAIAction(AActor* OtherActor = nullptr);
	UPROPERTY(EditAnywhere, Category="Destroy")
	bool bShouldDestroyAfterTrigger = false;
};
