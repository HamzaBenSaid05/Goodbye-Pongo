#include "Item/Interactable/TPP_ItemSwitch.h"

#include "Components/TPP_SoundComponent.h"
#include "EventProxies/TPP_AudioProxySubsystem.h"
#include "Item/Hazard/HazardManagers/TPP_HazardManager.h"

TObjectPtr<class UTPP_InteractAction_DA> ATPP_ItemSwitch::InteractWith()
{
	if (bIsAnimating)
	{
		return nullptr; 
	}
	return Super::InteractWith();
}

void ATPP_ItemSwitch::SwitchElectricity_Implementation()
{
	ImplementationBlueprintSwitch();

	if (ElectricityManagerRef)
		ElectricityManagerRef->OnSwitchActivationHandle(this);

	FVector WorldPos = this->GetTransform().TransformPosition(this->GetActorLocation());
	if (!bActive)
	{
		FTPP_AudioParameter Parameter = FTPP_AudioParameter();
		Parameter.Int = 0;
		SoundComponent->PlaySound(TagLever, GetActorLocation(), AudioComActive, Parameter);
		bActive = true;
	}
	else
	{
		FTPP_AudioParameter Parameter = FTPP_AudioParameter();
		Parameter.Int = 1;
		SoundComponent->PlaySound(TagLever, GetActorLocation(), AudioComDeactive, Parameter);
		bActive = false;
	}
}

void ATPP_ItemSwitch::SetHazardManager(class ATPP_ElectricityManager* ElectricityManager)
{
	ElectricityManagerRef = ElectricityManager;
}
