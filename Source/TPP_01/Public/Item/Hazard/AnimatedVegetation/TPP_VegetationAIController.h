// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TPP_VegetationAIController.generated.h"


class ACharacter;
/**
 * 
 */
UCLASS()
class TPP_01_API ATPP_VegetationAIController : public AAIController
{
	GENERATED_BODY()

public:
	ATPP_VegetationAIController();

protected:
	
	FGenericTeamId TeamID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* PerceptionComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TArray<ACharacter*> VisibleCharacters;
	UPROPERTY(VisibleAnywhere)
	TMap<ACharacter*, FVector> CharacterSightPositionMap;
	UPROPERTY()
	class UAISenseConfig_Sight* SightConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float TimerMarkPostion = 0.5f;
	UPROPERTY()
	int IterationBeforeShooting = 3;
	UPROPERTY()
	int IterationCount;
	
	UPROPERTY()
	FTimerHandle TimerScanPosition;
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
	void MarkCharacterLocation();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeToRun;
	
	virtual void OnPossess(APawn* InPawn) override;
	UPROPERTY()
	TObjectPtr<class ATPP_AnimatedVegetationPawn>	VegetationPawn;
	UPROPERTY()
	TObjectPtr<ACharacter> ClosestCharacter;

public:
	
	TMap<ACharacter*, FVector> GetCharacterMap() const {return CharacterSightPositionMap;}
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TSubclassOf<class ATPP_VegetationProjectile> ProjectileToShoot;
	
	virtual FGenericTeamId GetGenericTeamId() const override;
	void SetTeam(FGenericTeamId NewTeam);
	
	void ShootProjectile();

};
