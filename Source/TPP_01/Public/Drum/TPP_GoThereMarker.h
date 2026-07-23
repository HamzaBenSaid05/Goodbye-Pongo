// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPP_GoThereMarker.generated.h"

UCLASS()
class TPP_01_API ATPP_GoThereMarker : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	FVector ActiveMarker(const FVector& MarkerDirection, const FVector& StartPositionMarker, const float& MaxDistanceMarker, const float& StartHeight);

	UFUNCTION(BlueprintCallable)
	void DisableMarker();

	//====Movement====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marker")
	float Speed = 150.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marker")
	float MaxStepUpHeight = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marker")
	float MinStep = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marker")
	float MaxHeight = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marker")
	float MaxStepDownHeight = 10000.0f;
	UPROPERTY(EditAnywhere, Category = "Marker")
	UCurveFloat* SpeedCurve = nullptr;

	//====Material====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Marker")
	TObjectPtr<UMaterialInterface> MarkerMaterialBase;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor ValidColor = FLinearColor::Green;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor InvalidColor = FLinearColor::Red;

	UFUNCTION(BlueprintCallable, Category = "Marker")
	void SetColor(FLinearColor NewColor);

	void SetVisibilityMarker(bool Visible) const;

	bool GetStateMarker() const {return bIsActive;}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UNiagaraComponent* PlayingVFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UNiagaraComponent* PylonVFX;

protected:
	ATPP_GoThereMarker();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void Move(const float& DeltaSeconds);
	//====Tag====
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tags")
	FName MyTag = TEXT("NotScalable");

	//====Marker View====
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> MarkerMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MarkerMaterial;

	TObjectPtr<UStaticMeshComponent> GetMesh() const { return MarkerMesh; }
	FHitResult LineTraceByChannel(const FVector& Start, const FVector& End, const FCollisionQueryParams& QueryParams, ECollisionChannel TraceChannel) const;
	FHitResult LineTraceByObjectType(const FVector& Start,const FVector& End,const FCollisionQueryParams& QueryParams,const FCollisionObjectQueryParams& ObjectParams) const;
private:
	
	UPROPERTY(EditAnywhere, Category = "Marker")
	float ForwardOffset = 50.0f;
	UPROPERTY(EditAnywhere, Category = "Marker")
	float DownOffset = 20.0f;
	FVector CharacterDirection;
	FVector StartPosition;
	FVector InitialDirection;
	FVector MarkerBounds;

	UPROPERTY(EditAnywhere, Category = "Marker", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float AngleInfluence = 0.95f;
	float MaxDistanceInside = 1.0f;
	float MaxDistance;
	float DrumHeight;
	
	uint8 bIsActive : 1 = false;
	uint8 bCanClimb : 1 = true;
	uint8 bMoveForward : 1 = true;

};
