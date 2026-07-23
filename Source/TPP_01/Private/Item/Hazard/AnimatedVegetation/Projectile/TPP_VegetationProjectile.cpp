// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Hazard/AnimatedVegetation/Projectile/TPP_VegetationProjectile.h"

#include "Characters/TPP_CharacterBase.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Item/Interactable/TPP_ItemSwitch.h"

// Sets default values
ATPP_VegetationProjectile::ATPP_VegetationProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = MeshComponent;
	ProjectileMovement->InitialSpeed = 1500.f; 
	ProjectileMovement->MaxSpeed = 1500.f;     
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	
	//ProjectileBox->CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileBox"));
}

void ATPP_VegetationProjectile::BeginPlay()
{
	Super::BeginPlay();
	//ProjectileBox->OnComponentBeginOverlap.AddDynamic(this,&ThisClass::OnOverlapBegin);
}

void ATPP_VegetationProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ATPP_CharacterBase* Character = Cast<ATPP_CharacterBase>(OtherActor);
		if (Character)
		{
			//Call CharacterReset
			Destroy();
			return;
		}
		
		IIPowerSwitch::Execute_SwitchElectricity(OtherActor);
	}
}

void ATPP_VegetationProjectile::FireInDirection(const FVector& ShootDirection)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
	}
}

