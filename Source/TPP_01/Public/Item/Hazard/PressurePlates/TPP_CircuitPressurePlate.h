#pragma once

#include "CoreMinimal.h"
#include "TPP_PressurePlate.h"
#include "TPP_CircuitPressurePlate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FToggled, class ATPP_CircuitPressurePlate*, PressurePlate);

UCLASS()
class TPP_01_API ATPP_CircuitPressurePlate : public ATPP_PressurePlate
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void TogglePlateMeshGraphics(bool bShouldActivate);
	
	UFUNCTION(BlueprintCallable)
	void ToggleSurfacePlateMeshGraphics(bool bShouldActivate);
	
	UPROPERTY(BlueprintAssignable)
	FToggled OnToggled;

protected:
	virtual void BeginPlay() override;
	virtual void SwitchElectricity_Implementation() override;

private:
	UPROPERTY(EditDefaultsOnly)
	FLinearColor ActiveColor;
	FLinearColor StartingColor;
	UPROPERTY(EditDefaultsOnly)
	float ActiveEmissiveBoost;
	UPROPERTY(Transient)
	class UMaterialInstanceDynamic* MaterialInst;
	UPROPERTY(Transient)
	class UMaterialInstanceDynamic* SurfacePlateMaterialInst;
};
