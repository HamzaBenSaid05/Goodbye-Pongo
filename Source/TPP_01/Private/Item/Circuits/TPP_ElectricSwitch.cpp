#include "Item/Circuits/TPP_ElectricSwitch.h"

#include "Item/Hazard/HazardManagers/TPP_HazardManager.h"

void ATPP_ElectricSwitch::SwitchElectricity_Implementation()
{
	if (ElectricityManagerRef)
		ElectricityManagerRef->OnSwitchActivationHandle(this);
}

void ATPP_ElectricSwitch::SetHazardManager(class ATPP_ElectricityManager* ElectricityManager) 
{
	ElectricityManagerRef = ElectricityManager;
}
