#include "Item/Hazard/AnimatedVegetation/TPP_VegetationAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Item/Hazard/AnimatedVegetation/TPP_AnimatedVegetationPawn.h"
#include "Item/Hazard/AnimatedVegetation/Projectile/TPP_VegetationProjectile.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"

ATPP_VegetationAIController::ATPP_VegetationAIController()
{
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 1500.0f;
	SightConfig->LoseSightRadius = 1800.0f;
	SightConfig->PeripheralVisionAngleDegrees = 45.0f;
	SightConfig->SetMaxAge(5.0f);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	
	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
		this,
		&ATPP_VegetationAIController::OnTargetDetected
	);

	TeamID = FGenericTeamId(10);
}

void ATPP_VegetationAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	VegetationPawn = Cast<ATPP_AnimatedVegetationPawn>(InPawn);
	/*if (!BehaviorTreeToRun) return;
	if (InPawn)
	{
		RunBehaviorTree(BehaviorTreeToRun);
	}*/
}

FGenericTeamId ATPP_VegetationAIController::GetGenericTeamId() const
{
	return TeamID;
}

void ATPP_VegetationAIController::SetTeam(FGenericTeamId NewTeam)
{
	TeamID = NewTeam;
}

void ATPP_VegetationAIController::ShootProjectile()
{
	FVector SpawnLocation = VegetationPawn->GetActorLocation() +VegetationPawn->GetActorForwardVector() * 100.f;
	FRotator SpawnRotation = VegetationPawn->GetActorRotation();

	FVector ShootDirection = (ClosestCharacter->GetActorLocation() - SpawnLocation).GetSafeNormal();

	ATPP_VegetationProjectile* Projectile = GetWorld()->SpawnActor<ATPP_VegetationProjectile>(ProjectileToShoot,SpawnLocation,SpawnRotation);

	if (Projectile)
	{
		Projectile->FireInDirection(ShootDirection);
	}
}

void ATPP_VegetationAIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	
	if (!Actor) return;
	
	ACharacter* DetectedCharacter = Cast<ACharacter>(Actor);
	if (!DetectedCharacter)return;
	
	TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus);

	if (SenseClass == UAISense_Sight::StaticClass())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			CharacterSightPositionMap.Add(DetectedCharacter,Stimulus.StimulusLocation);
			
			if (!TimerScanPosition.IsValid())
			{
				GetWorld()->GetTimerManager().SetTimer(TimerScanPosition,this,&ThisClass::MarkCharacterLocation,TimerMarkPostion,true);
				IterationCount = 0;
			}
			
			UE_LOG(LogTemp, Warning, TEXT("Sight detected: %s"), *Actor->GetName());
		}
		else
		{
			CharacterSightPositionMap.Remove(DetectedCharacter);
			
			if (CharacterSightPositionMap.Num() == 0)
			{
				GetWorld()->GetTimerManager().ClearTimer(TimerScanPosition);
				IterationCount = 0;
			}
			UE_LOG(LogTemp, Warning, TEXT("Lost sight: %s"), *Actor->GetName());
		}
	}
}

void ATPP_VegetationAIController::MarkCharacterLocation()
{
	IterationCount++;
	float MinDistance = TNumericLimits<float>::Max();
	for (TPair<ACharacter*, FVector>& Elem : CharacterSightPositionMap)
	{
		ACharacter* Char = Elem.Key;
		if (!Char) continue;
		
		float Distance = FVector::Dist(VegetationPawn->GetActorLocation(), Char->GetActorLocation());
		
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			ClosestCharacter = Char;
		}
		
		FActorPerceptionBlueprintInfo Info;
		PerceptionComp->GetActorsPerception(Char, Info);

		for (FAIStimulus& Stim : Info.LastSensedStimuli)
		{
			if (Stim.Type == UAISense::GetSenseID<UAISense_Sight>())
			{
				CharacterSightPositionMap[Char] = Stim.StimulusLocation;
				DrawDebugSphere(GetWorld(), Stim.StimulusLocation, 10, 12, FColor::Red, false, 10);
			}
		}
	}
	
	if (IterationCount>= IterationBeforeShooting)
	{
		ShootProjectile();
		IterationCount = 0;
	}
}



