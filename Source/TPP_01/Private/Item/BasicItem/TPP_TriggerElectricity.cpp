// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BasicItem/TPP_TriggerElectricity.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Item/Hazard/HazardManagers/TPP_HazardManager.h"

// Sets default values
ATPP_TriggerElectricity::ATPP_TriggerElectricity()
{
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->InitBoxExtent(FVector(100.f, 100.f, 20.f));


	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionProfileName(TEXT("Trigger"));
	
	CollisionBox->OnComponentBeginOverlap.AddDynamic(
		this,
		&ThisClass::OnCollisionBeginOverlap
	);
}

void ATPP_TriggerElectricity::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ACharacter* Char = Cast<ACharacter>(OtherActor);	
		if (Char)
		{
			IIPowerSwitch::Execute_SwitchElectricity(this);
			Destroy();
		}
		
	}
}

void ATPP_TriggerElectricity::SwitchElectricity_Implementation()
{
	ElectricityManagerRef->OnSwitchActivationHandle(this);
}

void ATPP_TriggerElectricity::SetHazardManager(class ATPP_ElectricityManager* ElectricityManager)
{
	ElectricityManagerRef = ElectricityManager;
}


