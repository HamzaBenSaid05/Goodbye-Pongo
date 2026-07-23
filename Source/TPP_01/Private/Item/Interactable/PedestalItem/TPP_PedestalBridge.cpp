#include "Item/Interactable/PedestalItem/TPP_PedestalBridge.h"

ATPP_PedestalBridge::ATPP_PedestalBridge() { PrimaryActorTick.bCanEverTick = true; }

void ATPP_PedestalBridge::BeginPlay() { Super::BeginPlay(); }

void ATPP_PedestalBridge::EnableBridge_Implementation() {}

void ATPP_PedestalBridge::DisableBridge_Implementation() {}

void ATPP_PedestalBridge::EnableObject()
{
	Super::EnableObject();
	EnableBridge();
}

void ATPP_PedestalBridge::DisableObject()
{
	Super::DisableObject();
	DisableBridge();
}

void ATPP_PedestalBridge::Tick(float DeltaTime) { Super::Tick(DeltaTime); }
