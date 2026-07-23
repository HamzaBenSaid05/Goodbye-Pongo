// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Hazard/FloorTraps/ElectricFloor/TPP_ElectricFloorEnergySource.h"
#include "Components/BoxComponent.h"

void ATPP_ElectricFloorEnergySource::BeginPlay()
{
	Super::BeginPlay();
	if (bIsActive)
	{
		if (CollisionBox)
		{
			CollisionBox->SetCollisionEnabled(
				ECollisionEnabled::QueryOnly);
		}
	}
	else
	{
		if (CollisionBox)
		{
			CollisionBox->SetCollisionEnabled(
				ECollisionEnabled::NoCollision);
		}
	}
}

void ATPP_ElectricFloorEnergySource::SwitchPower_Implementation()
{
	bIsActive = !bIsActive;
	if (bIsActive)
	{
		if (CollisionBox)
		{
			CollisionBox->SetCollisionEnabled(
				ECollisionEnabled::QueryOnly);
		}
	}
	else
	{
		if (CollisionBox)
		{
			CollisionBox->SetCollisionEnabled(
				ECollisionEnabled::NoCollision);
		}
	}
}

void ATPP_ElectricFloorEnergySource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	DrawDebugState();
}

void ATPP_ElectricFloorEnergySource::DrawDebugState()
{
		if (!CollisionBox) return;
	
		FVector Center = CollisionBox->GetComponentLocation();
		FVector Extent = CollisionBox->GetScaledBoxExtent();
		FQuat Rotation = CollisionBox->GetComponentQuat();

		FColor DebugColor = bIsActive ? FColor::Green : FColor::Red;

		DrawDebugBox(
			GetWorld(),
			Center,
			Extent,
			Rotation,
			DebugColor,
			false,
			0.f,
			0,
			3.f
		);
}
