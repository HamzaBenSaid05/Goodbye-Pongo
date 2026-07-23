// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums/Item/OrbitType.h"
#include "Item/Interactable/TPP_InteractableItem.h"
#include "TPP_PedestalItemBase.generated.h"

UCLASS()
class TPP_01_API ATPP_PedestalItemBase : public ATPP_InteractableItem
{
	GENERATED_BODY()
	
public:
	ATPP_PedestalItemBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* DropBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* PickUpBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* PickUpPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orb")
	TObjectPtr<ATPP_InteractableItem> Orb;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Tag")
	FGameplayTag TagPedestal;
	UPROPERTY()
	UAudioComponent* AudioCompPedestal;
	UPROPERTY(EditAnywhere)
	OrbitType Type = OrbitType::Green;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsReactivable = true;
	bool bIsActivated;
	void SnapItem(class ATPP_InteractableItem* Item);

protected:
	UFUNCTION()
	virtual void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                                     bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void EnableObject();
	UFUNCTION()
	virtual void DisableObject();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Emitter;
};
