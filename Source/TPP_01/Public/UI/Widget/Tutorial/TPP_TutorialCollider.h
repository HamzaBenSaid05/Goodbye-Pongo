// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UI/HUD/TPP_HUDGame.h"
#include "TPP_TutorialCollider.generated.h"

class UBoxComponent;
class UWidgetComponent;

UCLASS()
class TPP_01_API ATPP_TutorialCollider : public AActor
{
	GENERATED_BODY()

public:

	ATPP_TutorialCollider();
	
protected:

	UPROPERTY(EditAnywhere, Category = "TutorialTime")
	float DelayBeforeTutorialActivation = 2.0f;

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	ECharacterTutorial CharTutorial;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> ColliderBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UWidgetComponent> WidgetComponent;
	UPROPERTY()
	TObjectPtr<class ATPP_HUDGame> HUDGame;
	UPROPERTY()
	TObjectPtr<class UTPP_TutorialWidget> TutorialWidget;
	
	FTimerHandle TimerHandle_MovementCheck;
	FTimerHandle TimerHandle_InTriggerCheck;
	FVector StartLocation2D;
	UPROPERTY(EditAnywhere, Category = "Tutorial | Movement")
	float MovementTolerance = 50.0f;
	void CheckMovementTimeout();
	void CheckDelayTutorialActiovation();
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void HandleMovement();
	void HandleCrouch();
	void HandleJump();
	void HandlePush();
	void HandlePet();
	void HandleComeHere();
	void HandleStop();
	void HandleInteract();
	void HandleGoThere();
	void HandleNone();

	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void OnMovementEventReceived();

	UFUNCTION()
	void OnPetFinishedTutotial();
public:
	UBoxComponent* GetColliderBox() const { return ColliderBox; }
	UWidgetComponent* GetWidgetComponent() const { return WidgetComponent; }
	
	UFUNCTION(BlueprintNativeEvent, Category="Tutorial")
	void TriggerTutorial();
	void TriggerTutorial_Implementation();
	UFUNCTION(BlueprintNativeEvent, Category="Tutorial")
	void TriggerTutorialOut();
	void TriggerTutorialOut_Implementation();
	
	void HandleTutorialTrigger(ECharacterTutorial CharacterTutorial);

private:
	bool bHasBindedOnce;
};
