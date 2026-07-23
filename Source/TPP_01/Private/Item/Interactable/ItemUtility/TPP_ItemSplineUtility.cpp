// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Interactable/ItemUtility/TPP_ItemSplineUtility.h"
#include "Components/SplineComponent.h"


ATPP_ItemSplineUtility::ATPP_ItemSplineUtility()
{
 	
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;
	PrimaryActorTick.bCanEverTick = false;

}


