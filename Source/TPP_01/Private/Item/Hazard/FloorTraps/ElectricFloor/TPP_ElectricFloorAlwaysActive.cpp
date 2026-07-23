// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Hazard/FloorTraps/ElectricFloor/TPP_ElectricFloorAlwaysActive.h"

#include "Components/BoxComponent.h"

void ATPP_ElectricFloorAlwaysActive::DrawDebugState()
{
	if (!CollisionBox) return;
	
	FVector Center = CollisionBox->GetComponentLocation();
	FVector Extent = CollisionBox->GetScaledBoxExtent();
	FQuat Rotation = CollisionBox->GetComponentQuat();

	DrawDebugBox(
		GetWorld(),
		Center,
		Extent,
		Rotation,
		FColor::Green,
		false,
		0.f,
		0,
		3.f
	);
}

void ATPP_ElectricFloorAlwaysActive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	DrawDebugState();
}
