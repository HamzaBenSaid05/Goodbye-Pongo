#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Sound/SoundSubmix.h"
#include "SoundSystem/Data/TPP_AudioRow.h"
#include "Subsystems/WorldSubsystem.h"
#include "TPP_AudioProxySubsystem.generated.h"

struct FTPP_AudioRow;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(
	FAudioTableEventDelegate,
	FTPP_AudioRow&, Row,
	class UAudioComponent*&, AudioComp,
	FVector&, Location,
	FTPP_AudioParameter&, Parameter,
	USceneComponent*, TargetSource
);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FAudioTableEventDelegateUI,
	FGameplayTag, Tag,
	FTPP_AudioParameter, Parameter,
	USoundSubmix*,Submix,
	ButtonType, Button
);

UCLASS(BlueprintType, Blueprintable)
class TPP_01_API UTPP_AudioProxySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FAudioTableEventDelegateUI OnAudioTableEventUI;
	FAudioTableEventDelegate OnAudioTableEvent;
	
	UFUNCTION(BlueprintCallable)
	void NotifyAudioTableEventUI(FGameplayTag Tag,struct FTPP_AudioParameter Parameter,USoundSubmix* Submix,ButtonType Button) const;
	void NotifyAudioTableEvent(struct FTPP_AudioRow & Row, class UAudioComponent *& AudioComp, FVector & Location,struct FTPP_AudioParameter& Parameter, class USceneComponent * SourceActor) const;

};
