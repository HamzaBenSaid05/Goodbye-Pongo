// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "TPP_InteractableItem.generated.h"

USTRUCT(BlueprintType)
struct FAttachPointData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Spawn Points", meta=(MakeEditWidget="true"))
	FVector Position;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FRotator Rotation;

	UPROPERTY(EditAnywhere, Category="Spawn Points", meta=(MakeEditWidget="true"))
	FVector LocationToVisualizeWidget;
};

UENUM(BlueprintType)
enum class ETPP_FaceDir: uint8
{
	None UMETA(DisplayName="None"),
	Front UMETA(DisplayName="Front"),
	Back UMETA(DisplayName="Back"),
	Right UMETA(DisplayName="Right"),
	Left UMETA(DisplayName="Left"),
	Top UMETA(DisplayName="Top"),
	Bottom UMETA(DisplayName="Bottom"),
};

USTRUCT(BlueprintType)
struct FFaceComponents
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Interact IK")
	TObjectPtr<UArrowComponent> FirstComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Interact IK")
	TObjectPtr<UArrowComponent> SecondComponent = nullptr;
};

USTRUCT(BlueprintType)
struct FFaceComponentEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interact IK", meta=(UseComponentPicker="true"))
	ETPP_FaceDir Face = ETPP_FaceDir::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interact IK", meta=(UseComponentPicker="true"))
	FFaceComponents FaceComponents;
};

UCLASS()
class TPP_01_API ATPP_InteractableItem : public AActor
{
	GENERATED_BODY()

public:
	ATPP_InteractableItem();

	//====Audio====
	UFUNCTION()
	void PlayDropSound();
	void PlayPushSound();
	void DisablePushSound();

	UPROPERTY(EditAnywhere)
	bool bHasToActivateGravity;

	bool GetClosestAttachPoint(const FVector& PlayerLocation, float Radius, FAttachPointData& OutPoint);

	UFUNCTION()
	FORCEINLINE class UBoxComponent* GetBoxNavMesh() { return BoxNavMesh; };

	UFUNCTION()
	void SetItemCollision(bool SimulatedPhysics, ECollisionEnabled::Type TypeCollison);

	//----Metodo to call For Interaction---
	virtual TObjectPtr<class UTPP_InteractAction_DA> InteractWith();

	void SetWidgetInteractLocation(FAttachPointData SelectedAttachPointData);
	void DeactiveWidgetInteract();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return Mesh; }

	FORCEINLINE TObjectPtr<class UTPP_InteractAction_DA> GetInteractData() const { return InteractData; }

	virtual void UpdateItemInteraction(const FVector& PlayerInput, const FVector& ForwardVector);

	//AI Methods
	virtual bool IsAIInteractionCompleted() const;

	virtual void EndAIInteraction();

	UPROPERTY(EditAnywhere, Category="Spawn Points", meta=(MakeEditWidget="true"))
	FVector NewStartDragPoint;
	UPROPERTY(EditAnywhere, Category="Spawn Points", meta=(MakeEditWidget="true"))
	FVector LocationToReach;
	UPROPERTY(EditAnywhere, Category="Spawn Points", meta=(MakeEditWidget="true"))
	TArray<FAttachPointData> AttachPoints;
	UPROPERTY()
	FAttachPointData SavedAttachPointData;
	UPROPERTY()
	FVector CoordinatesStartInteraction;
	UPROPERTY()
	FVector CoordinatesMediumPointInteraction;

	UFUNCTION()
	void UpdateRotationToReach();

	UFUNCTION(BlueprintCallable)
	void SetInteract(bool active);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetCanInteract() const{return BCanInteract;}

	//End AI Methods
	UFUNCTION()
	FORCEINLINE FVector GetDirectionToDrag() const { return bHasReachFinalPoint ? StartLocation : FinalPongoLocation; }

	UPROPERTY()
	bool bHasReachFinalPoint;
	UPROPERTY()
	FVector FinalPongoLocation;
	UPROPERTY(EditAnywhere, Category="Fall")
	bool bHasToFall;
	UPROPERTY(EditAnywhere, Category="ItemOffset")
	float StartFallOffset = 0.f;

	void DestroyItem();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interact IK|Hand")
	float DesiredGripAlpha = 0.65f;
	UFUNCTION(BlueprintCallable, Category="Interact IK")
	bool GetHandSurfaceTransform(
		ETPP_FaceDir Dir,
		bool bLeftHand,
		FVector& OutLocation,
		FRotator& OutRotation) const;
	UFUNCTION(BlueprintCallable, Category="Interact IK")
	FFaceComponents GetSceneComponentFromDir(ETPP_FaceDir Dir);
	UFUNCTION(BlueprintCallable, Category="Interact Ik")
	ETPP_FaceDir GetFaceDir(AActor* TargetActor);
	UPROPERTY(EditAnywhere, Category="Interact IK|Rotation")
	FRotator OffsetRotator = FRotator(0.f, 0.f, 0.f);
	//====Audio====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Audio")
	TObjectPtr<class UTPP_SoundComponent> SoundComponent;
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	FGameplayTag TagItem;
	UPROPERTY()
	UAudioComponent* AudioCompDrop;
	UPROPERTY()
	UAudioComponent* AudioCompPush;
	//====Audio====
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interact")
	TObjectPtr<class UTPP_InteractAction_DA> InteractData;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(BlueprintReadOnly, Category = "Init")
	FVector StartLocation;
	UPROPERTY(BlueprintReadOnly, Category = "Init")
	FRotator StartRotation;

	//UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	TObjectPtr<class UWidgetComponent> WidgetComp;
	
	UFUNCTION()
	void SaveStartLocation();

	UFUNCTION(BlueprintCallable)
	void ResetStartLocation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NavMesh")
	TObjectPtr<class UBoxComponent> BoxNavMesh;
	bool BCanInteract = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WidgetOffset")
	float WidgetOffsetZ = 20.f;
	virtual void Tick(float DeltaTime) override;

	// Interact IK - Arrow Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interact IK")
	TObjectPtr<UArrowComponent> Front_L;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interact IK")
	TObjectPtr<UArrowComponent> Front_R;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interact IK")
	TObjectPtr<UArrowComponent> Back_L;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interact IK")
	TObjectPtr<UArrowComponent> Back_R;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interact IK")
	TObjectPtr<UArrowComponent> Right_L;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interact IK")
	TObjectPtr<UArrowComponent> Right_R;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interact IK")
	TObjectPtr<UArrowComponent> Left_L;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interact IK")
	TObjectPtr<UArrowComponent> Left_R;
};
