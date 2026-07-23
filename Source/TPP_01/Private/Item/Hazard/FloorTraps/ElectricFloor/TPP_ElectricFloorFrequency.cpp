// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Hazard/FloorTraps/ElectricFloor/TPP_ElectricFloorFrequency.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"



void ATPP_ElectricFloorFrequency::BeginPlay()
{
	Super::BeginPlay();

	ActivationFunction();
}

void ATPP_ElectricFloorFrequency::ActivationFunction()
{
	ActivateTrap();
}
void ATPP_ElectricFloorFrequency::DeactivationFunction()
{
	DeactivateTrap();
}

void ATPP_ElectricFloorFrequency::ActivateTrap()
{
	//Degbug
	bIsActive = true;
	
	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(
			ECollisionEnabled::QueryOnly);
	}

	GetWorldTimerManager().SetTimer(
		FrequencyTimer,
		this,
		&ATPP_ElectricFloorFrequency::DeactivateTrap,
		ActiveTime,
		false);
}

void ATPP_ElectricFloorFrequency::DeactivateTrap()
{
	//Degbug
	bIsActive = false;

	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(
			ECollisionEnabled::NoCollision);
	}

	GetWorldTimerManager().SetTimer(
		FrequencyTimer,
		this,
		&ATPP_ElectricFloorFrequency::ActivateTrap,
		InactiveTime,
		false);
}

void ATPP_ElectricFloorFrequency::ToggleTrap()
{
	GetWorldTimerManager().ClearTimer(FrequencyTimer);

	if (bIsActive)
	{
		DeactivateTrap();
	}
	else
	{
		ActivateTrap();
	}
}

void ATPP_ElectricFloorFrequency::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugState();
}

void ATPP_ElectricFloorFrequency::DrawDebugState()
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