// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPP_InteractableItem.h"
#include "TPP_OrbItem.generated.h"

UCLASS()
class TPP_01_API ATPP_OrbItem : public ATPP_InteractableItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class ATPP_PedestalItem> PedestalItem;
	// Quick fix for alpha To remove in later version
	void SetPedestal(TObjectPtr<class ATPP_PedestalItem> Item) {PedestalItem = Item;}
	TObjectPtr<class ATPP_PedestalItem> GetPedestal() { return PedestalItem; }
protected:
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	FGameplayTag TagOrb;
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	int OrbParameter;
	UPROPERTY()
	UAudioComponent* AudioComOrb;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
