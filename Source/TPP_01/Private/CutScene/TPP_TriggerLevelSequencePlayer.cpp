#include "CutScene/TPP_TriggerLevelSequencePlayer.h"

#include "Components/ShapeComponent.h"
#include "Gameplay/GameModes/TPP_GameMode.h"
#include "Kismet/GameplayStatics.h"

ATPP_TriggerLevelSequencePlayer::ATPP_TriggerLevelSequencePlayer()
{
	GetCollisionComponent()->SetGenerateOverlapEvents(true);
	GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCollisionComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCollisionComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	GetCollisionComponent()->SetCanEverAffectNavigation(false);
	GetCollisionComponent()->SetHiddenInGame(false);
	PrimaryActorTick.bCanEverTick = false;
}

void ATPP_TriggerLevelSequencePlayer::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComp,
                                                                    AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                                    int32 OtherBodyIndex, bool bFromSweep,
                                                                    const FHitResult& SweepResult)
{
	LevelSequencePlayer = Cast<ATPP_GameMode>(GetWorld()->GetAuthGameMode())->GetLevelSequencePlayer();
	LevelSequencePlayer->PlaySequence(GetSequence(LevelSequenceSoft));
	// Disable the trigger to prevent multiple overlaps
	GetCollisionComponent()->SetGenerateOverlapEvents(false);
	Destroy();
}

void ATPP_TriggerLevelSequencePlayer::BeginPlay()
{
	Super::BeginPlay();
	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &ATPP_TriggerLevelSequencePlayer::OnBeginOverlap);
}
