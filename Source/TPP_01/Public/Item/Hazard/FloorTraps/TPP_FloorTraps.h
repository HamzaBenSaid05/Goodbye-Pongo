// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Hazard/TPP_Hazard.h"
#include "TPP_FloorTraps.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API ATPP_FloorTraps : public ATPP_Hazard
{
	GENERATED_BODY()

protected:
	ATPP_FloorTraps();
	
	virtual void BeginPlay() override;
	
	UPROPERTY()
	class ATPP_GameMode* GameModeRef;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mesh")
	class UStaticMeshComponent* TrapMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Collision")
	class UBoxComponent* CollisionBox;
	
	UFUNCTION()
	void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
};
