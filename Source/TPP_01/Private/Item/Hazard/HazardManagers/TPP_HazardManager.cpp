// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Hazard/HazardManagers/TPP_HazardManager.h"

#include "Gameplay/GameModes/TPP_GameMode.h"
#include "Interfaces/Hazards/IPowerSwitch.h"
#include "Kismet/GameplayStatics.h"

void ATPP_ElectricityManager::BeginPlay()
{
	Super::BeginPlay();

	for (auto& Elem : ManagedItems)
	{
		AActor* SwitchActor = Elem.Key;
		if (!SwitchActor) continue;

		IIPowerSwitch* SwitchInterface = Cast<IIPowerSwitch>(SwitchActor);
		if (SwitchInterface)
		{
			SwitchInterface->SetHazardManager(this);

			GEngine->AddOnScreenDebugMessage
			(
				-1,
				5.f,
				FColor::Green,
				FString::Printf(TEXT("Registrer PowerSwitch: %s"), *SwitchActor->GetName())
			);
		}
	}
}

void ATPP_ElectricityManager::OnSwitchActivationHandle(AActor* SwitchActor)
{
	if (!SwitchActor) return;

	FItemToLinkPower* FoundData = ManagedItems.Find(SwitchActor);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Sto cercando: %s"), *SwitchActor->GetName()));

	if (FoundData)
	{
		IIPowerSwitch* SwitchInterface = Cast<IIPowerSwitch>(SwitchActor);
		if (SwitchInterface)
		{
			for (AActor* ConnectedActor : FoundData->ItemTournableOn)
			{
				if (ConnectedActor)
				{
					INeedPowerSwitch::Execute_SwitchPower(ConnectedActor);
				}
			}
		}
	}
}
