// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Hazard/FloorTraps/TPP_FloorTraps.h"

#include "Characters/TPP_CharacterBase.h"
#include "Components/BoxComponent.h"
#include "Gameplay/GameModes/TPP_GameMode.h"
#include "Kismet/GameplayStatics.h"

ATPP_FloorTraps::ATPP_FloorTraps()
{
	TrapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapMesh"));
	RootComponent = TrapMesh;
	
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	
	CollisionBox->InitBoxExtent(FVector(100.f, 100.f, 20.f));


	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionProfileName(TEXT("Trigger"));
	
	CollisionBox->OnComponentBeginOverlap.AddDynamic(
		this,
		&ATPP_FloorTraps::OnCollisionBeginOverlap
	);
	
}

void ATPP_FloorTraps::BeginPlay()
{
	Super::BeginPlay();
	
	GameModeRef = Cast<ATPP_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}

void ATPP_FloorTraps::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                              int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GameModeRef)
	{
		ATPP_CharacterBase* Char = Cast<ATPP_CharacterBase>(OtherActor);
		if (Char)
		{
			Char->Kill();
		}
	}
}



