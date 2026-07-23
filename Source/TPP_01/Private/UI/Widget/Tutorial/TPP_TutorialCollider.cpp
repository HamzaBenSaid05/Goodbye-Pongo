// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/Tutorial/TPP_TutorialCollider.h"

#include "Characters/Player/TPP_PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widget/Tutorial/TPP_TutorialWidget.h"
#include "Gameplay/GameInstances/TPP_GameInstance.h"
#include "Gameplay/GameModes/TPP_GameMode.h"
#include "Characters/Pongo/TPP_Pongo.h"


// Sets default values
ATPP_TutorialCollider::ATPP_TutorialCollider()
{
	ColliderBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ColliderBox"));
	RootComponent = ColliderBox;
	ColliderBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ColliderBox->SetCollisionObjectType(ECC_WorldStatic);
	ColliderBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	ColliderBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetDrawAtDesiredSize(true);
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}



void ATPP_TutorialCollider::BeginPlay()
{
	Super::BeginPlay();

	if (ColliderBox)
	{
		ColliderBox->OnComponentBeginOverlap.AddDynamic(this, &ATPP_TutorialCollider::OnOverlapBegin);
		ColliderBox->OnComponentEndOverlap.AddDynamic(this, &ATPP_TutorialCollider::OnOverlapEnd);
	}
	

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->GetHUD())
		HUDGame = Cast<ATPP_HUDGame>(PC->GetHUD());
	
	TutorialWidget = Cast<UTPP_TutorialWidget>(WidgetComponent->GetWidget());
	
}

void ATPP_TutorialCollider::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                           bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ATPP_PlayerCharacter>(OtherActor))
	{
		if (HUDGame)
		{
			UTPP_GameInstance* GameInstanceRef = Cast<UTPP_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			
			if(GameInstanceRef->GetDeactivateTutorial())
				return;

			TutorialWidget->SetCurrentTutorialData(HUDGame->GetDataTutorial(CharTutorial));
			HandleTutorialTrigger(CharTutorial);
			TutorialWidget->InizializeTorialWidget();
		}
	}
}

void ATPP_TutorialCollider::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<ATPP_PlayerCharacter>(OtherActor))
	{
		if (HUDGame)
		{
			UTPP_GameInstance* GameInstanceRef = Cast<UTPP_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			if (GameInstanceRef->GetDeactivateTutorial())
				return;

			TutorialWidget->SetCurrentTutorialData(HUDGame->GetDataTutorial(CharTutorial));
			TriggerTutorialOut();
		}
	}
}


void ATPP_TutorialCollider::CheckMovementTimeout()
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter)
	{
		FVector CurrentLocation2D = PlayerCharacter->GetActorLocation();
		CurrentLocation2D.Z = 0.0f;
		
		float Distance = FVector::Dist(StartLocation2D, CurrentLocation2D);
		
		if (Distance < MovementTolerance)
		{
			TriggerTutorial();
			UE_LOG(LogTemp, Warning, TEXT("On: %f"), Distance);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Off."));
		}
	}
}

void ATPP_TutorialCollider::CheckDelayTutorialActiovation()
{
	TriggerTutorial();
}

void ATPP_TutorialCollider::HandleMovement()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC == nullptr) return;

	if (ATPP_HUDGame* GameHud = Cast<ATPP_HUDGame>(PC->GetHUD()))
	{
		GameHud->OnCutSceneSkipped.AddDynamic(this, &ThisClass::OnMovementEventReceived);
	}
}
void ATPP_TutorialCollider::HandleCrouch() {
	
	GetWorldTimerManager().SetTimer(
		TimerHandle_InTriggerCheck,
		this,
		&ATPP_TutorialCollider::CheckDelayTutorialActiovation,
		DelayBeforeTutorialActivation,
		false
	);
}
void ATPP_TutorialCollider::HandleJump() {
	
	GetWorldTimerManager().SetTimer(
		TimerHandle_InTriggerCheck,
		this,
		&ATPP_TutorialCollider::CheckDelayTutorialActiovation,
		DelayBeforeTutorialActivation,
		false
	);
}
void ATPP_TutorialCollider::HandlePush() {}
void ATPP_TutorialCollider::HandlePet() {}
void ATPP_TutorialCollider::HandleComeHere() {}
void ATPP_TutorialCollider::HandleStop() {}
void ATPP_TutorialCollider::HandleInteract() {}
void ATPP_TutorialCollider::HandleGoThere() {}
void ATPP_TutorialCollider::HandleNone() {}

void ATPP_TutorialCollider::OnMovementEventReceived()
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    
	if (PlayerCharacter)
	{
		StartLocation2D = PlayerCharacter->GetActorLocation();
		StartLocation2D.Z = 0.0f;
		
		GetWorldTimerManager().SetTimer(
			TimerHandle_MovementCheck, 
			this, 
			&ATPP_TutorialCollider::CheckMovementTimeout, 
			DelayBeforeTutorialActivation,
			false 
		);
	}
}

void ATPP_TutorialCollider::OnPetFinishedTutotial()
{
	TriggerTutorialOut();

	ATPP_GameMode* GameModeRef = Cast<ATPP_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameModeRef && GameModeRef->GetPongoRef())
	{
		GameModeRef->GetPongoRef()->OnPetFinished.RemoveDynamic(this, &ThisClass::OnPetFinishedTutotial);
	}
}

void ATPP_TutorialCollider::TriggerTutorial_Implementation() {}
void ATPP_TutorialCollider::TriggerTutorialOut_Implementation(){}
void ATPP_TutorialCollider::HandleTutorialTrigger(ECharacterTutorial CharacterTutorial)
{
	switch (CharacterTutorial)
	{
	case ECharacterTutorial::Movement:
		UE_LOG(LogTemp, Warning, TEXT("Tutorial: Movement"));
		HandleMovement();
		break;

	case ECharacterTutorial::Crouch:
		UE_LOG(LogTemp, Warning, TEXT("Tutorial: Crouch"));
		HandleCrouch();
		break;

	case ECharacterTutorial::Jump:
		UE_LOG(LogTemp, Warning, TEXT("Tutorial: Jump"));
		HandleJump();
		break;

	case ECharacterTutorial::Push:
		UE_LOG(LogTemp, Warning, TEXT("Tutorial: Push"));
		TriggerTutorial();
		break;

	case ECharacterTutorial::Pet:
		UE_LOG(LogTemp, Warning, TEXT("Tutorial: Pet"));
		if (HUDGame) {
			if (!HUDGame->GetHasPongoBeingPetOnce()) {

				if (!bHasBindedOnce) {
					ATPP_GameMode* GameModeRef = Cast<ATPP_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));

					if (GameModeRef) {
						if (GameModeRef->GetPongoRef()) {
							GameModeRef->GetPongoRef()->OnPetFinished.AddDynamic(this, &ThisClass::OnPetFinishedTutotial);
						}
					}
					bHasBindedOnce = true;
				}
				
				TriggerTutorial();
			}
		}
		break;

	case ECharacterTutorial::ComeHere:
		UE_LOG(LogTemp, Warning, TEXT("Tutorial: ComeHere"));
		TriggerTutorial();
		break;

	case ECharacterTutorial::Stop:
		UE_LOG(LogTemp, Warning, TEXT("Tutorial: Stop"));
		TriggerTutorial();
		break;

	case ECharacterTutorial::Interact:
		UE_LOG(LogTemp, Warning, TEXT("Tutorial: Interact"));
		TriggerTutorial();
		break;

	case ECharacterTutorial::GoThere:
		UE_LOG(LogTemp, Warning, TEXT("Tutorial: GoThere"));
		TriggerTutorial();
		break;
	case ECharacterTutorial::JaneInteract:
		UE_LOG(LogTemp, Warning, TEXT("Tutorial: GoThere"));
		TriggerTutorial();
		break;
	case ECharacterTutorial::None:
	default:
		UE_LOG(LogTemp, Warning, TEXT("Tutorial: None"));
		break;
	}
}

