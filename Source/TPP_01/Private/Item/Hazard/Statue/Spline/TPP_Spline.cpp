// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Hazard/Statue/Spline/TPP_Spline.h"

#include "Components/SplineComponent.h"

// Sets default values
ATPP_Spline::ATPP_Spline()
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;
	SplineComponent->SetMobility(EComponentMobility::Movable);
	SplineComponent->bDrawDebug = true;
	PrimaryActorTick.bCanEverTick = false;

}

