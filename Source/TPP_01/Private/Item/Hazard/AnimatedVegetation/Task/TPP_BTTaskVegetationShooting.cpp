// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Hazard/AnimatedVegetation/Task/TPP_BTTaskVegetationShooting.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Item/Hazard/AnimatedVegetation/TPP_AnimatedVegetationPawn.h"
#include "Item/Hazard/AnimatedVegetation/TPP_VegetationAIController.h"
#include "Item/Hazard/AnimatedVegetation/Projectile/TPP_VegetationProjectile.h"

EBTNodeResult::Type UTPP_BTTaskVegetationShooting::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1,10,FColor::Yellow,TEXT("1"));
	}
	ATPP_VegetationAIController* VegetationAIController = Cast<ATPP_VegetationAIController>(AIController);
	if (!VegetationAIController) return EBTNodeResult::Failed;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1,10,FColor::Yellow,TEXT("2"));
	}
	ATPP_AnimatedVegetationPawn* VegetationPawn = Cast<ATPP_AnimatedVegetationPawn>(VegetationAIController->GetPawn());
	if (!VegetationPawn) return EBTNodeResult::Failed;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1,10,FColor::Yellow,TEXT("3"));
	}
	UObject* Obj = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName);
	ACharacter* ClosestCharacter = Cast<ACharacter>(Obj);
	if (!ClosestCharacter) return EBTNodeResult::Failed;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1,10,FColor::Yellow,TEXT("4"));
	}	
	
	FVector SpawnLocation = VegetationPawn->GetActorLocation() +VegetationPawn->GetActorForwardVector() * 100.f;
	FRotator SpawnRotation = VegetationPawn->GetActorRotation();

	FVector ShootDirection = (ClosestCharacter->GetActorLocation() - SpawnLocation).GetSafeNormal();

	ATPP_VegetationProjectile* Projectile = GetWorld()->SpawnActor<ATPP_VegetationProjectile>(
		VegetationAIController->ProjectileToShoot, SpawnLocation, SpawnRotation);

	if (Projectile)
	{
		Projectile->FireInDirection(ShootDirection);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("bCanShoot",false);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
