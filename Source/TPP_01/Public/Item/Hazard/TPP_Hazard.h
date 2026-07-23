// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPP_Hazard.generated.h"

UCLASS()
class TPP_01_API ATPP_Hazard : public AActor
{
	GENERATED_BODY()
	
	
public:
	ATPP_Hazard();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayAnimation();
	protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<class UTPP_SoundComponent> SoundComponent;
};
