// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Hazard/TPP_Hazard.h"

#include "Components/TPP_SoundComponent.h"

ATPP_Hazard::ATPP_Hazard()
{
	SoundComponent = CreateDefaultSubobject<UTPP_SoundComponent>(TEXT("SoundComponent"));
}
