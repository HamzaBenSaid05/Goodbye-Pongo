#include "Item/Hazard/PressurePlates/TPP_CircuitPressurePlate.h"

void ATPP_CircuitPressurePlate::TogglePlateMeshGraphics(bool bShouldActivate)
{
	FLinearColor TargetColor = bShouldActivate ? ActiveColor : StartingColor;

	MaterialInst->SetVectorParameterValue("Color", TargetColor);
}

void ATPP_CircuitPressurePlate::ToggleSurfacePlateMeshGraphics(bool bShouldActivate) 
{
	float TargetBoost = bShouldActivate ? ActiveEmissiveBoost : 1;
	SurfacePlateMesh->SetCustomPrimitiveDataFloat(2, TargetBoost);
}

void ATPP_CircuitPressurePlate::BeginPlay()
{
	Super::BeginPlay();

	MaterialInst = PlateMesh->CreateDynamicMaterialInstance(0);
	MaterialInst->GetVectorParameterValue(FName("Color"), StartingColor);
}

void ATPP_CircuitPressurePlate::SwitchElectricity_Implementation()
{
	bIsActivated = !bIsActivated;
	ToggleSurfacePlateMeshGraphics(bIsActivated);
	
	OnToggled.Broadcast(this);
}
