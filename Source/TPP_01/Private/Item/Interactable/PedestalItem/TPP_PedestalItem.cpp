#include "Item/Interactable/PedestalItem/TPP_PedestalItem.h"

#include "DataAsset/InteractActions/TPP_Orb_DA.h"
#include "Item/Hazard/HazardManagers/TPP_HazardManager.h"
#include "Item/Interactable/TPP_OrbItem.h"

ATPP_PedestalItem::ATPP_PedestalItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATPP_PedestalItem::SwitchElectricity_Implementation()
{
	ElectricityManagerRef->OnSwitchActivationHandle(this);
}

void ATPP_PedestalItem::SetHazardManager(class ATPP_ElectricityManager* ElectricityManager)
{
	ElectricityManagerRef = ElectricityManager;
}

void ATPP_PedestalItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ATPP_PedestalItem::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}

void ATPP_PedestalItem::EnableObject()
{
	Super::EnableObject();
	SetActorTickEnabled(true);
	bIsActivated = true;
	IIPowerSwitch::Execute_SwitchElectricity(this);
}

void ATPP_PedestalItem::DisableObject()
{
	Super::DisableObject();
	IIPowerSwitch::Execute_SwitchElectricity(this);
	SetActorTickEnabled(false);
	bIsActivated = false;
}

void ATPP_PedestalItem::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATPP_OrbItem* Item = Cast<ATPP_OrbItem>(OtherActor);
	if (!Item) return;
	if (!Item->GetCanInteract()) return;
	UTPP_Orb_DA* Data = Cast<UTPP_Orb_DA>(Item->GetInteractData());
	if (!Data || Data->GetColorType() != Type) return;
	Super::OnCollisionBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
