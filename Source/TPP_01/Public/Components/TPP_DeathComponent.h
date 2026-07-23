#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "Enums/TPP_DeathInfo.h"
#include "TPP_DeathComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDead, FDeathInfo, DeathInfo);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPP_01_API UTPP_DeathComponent : public UActorComponent
{
	GENERATED_BODY()

	UTPP_DeathComponent();

public:
	UFUNCTION(BlueprintNativeEvent)
	void Killed(const FDeathInfo& DeathInfo);

	UPROPERTY(BlueprintAssignable)
	FOnDead OnDead;
	
	bool bIsDying;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void Dissolve(class USkeletalMeshComponent* Mesh);

private:
	// === Dissolve ===
	UPROPERTY(EditDefaultsOnly, Category = "Death|Dissolve")
	UCurveFloat* DissolveCurve;
	UPROPERTY()
	TArray<class UMaterialInstanceDynamic*> DynamicMaterialInst;

	UPROPERTY()
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTimelineProgress;
	FOnTimelineEvent DissolveTimelineFinishedEvent;

	UFUNCTION()
	void DissolveProgress(const float Output);
	// ================

	UFUNCTION()
	void FinishDeath();

	FDeathInfo SavedDeathInfo;
};
