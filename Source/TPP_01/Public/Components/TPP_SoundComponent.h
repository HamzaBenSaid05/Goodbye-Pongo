// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoundSystem/Data/TPP_AudioRow.h"
#include "TPP_SoundComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPP_01_API UTPP_SoundComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* Table;
	UFUNCTION(BluePrintCallable)
	USoundBase* FindSound(FGameplayTag Tag);
	FTPP_AudioRow* FindGamplayTag(FGameplayTag Tag) const;
	UFUNCTION(BluePrintCallable)
	void PlaySound(FGameplayTag Tag, FVector SourceLocation, UAudioComponent*& AudioComp, FTPP_AudioParameter Parameter = FTPP_AudioParameter(), USceneComponent* SourceActor = nullptr);
	void SetAudioComponent(UAudioComponent*& AudioComponent, FGameplayTag Tag);
	
};
