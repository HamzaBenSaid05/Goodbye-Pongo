#pragma once

#include "CoreMinimal.h"
#include "TPP_InteractableItem.h"
#include "Interfaces/Hazards/IPowerSwitch.h"
#include "TPP_ItemSwitch.generated.h"

UCLASS()
class TPP_01_API ATPP_ItemSwitch : public ATPP_InteractableItem, public IIPowerSwitch
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Audio Tag")
	FGameplayTag TagLever;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activation")
	bool bActive = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activation")
	bool bIsAnimating = false;
	UPROPERTY()
	UAudioComponent* AudioComActive;
	UPROPERTY()
	UAudioComponent* AudioComDeactive;
	virtual TObjectPtr<class UTPP_InteractAction_DA> InteractWith() override;
	virtual void SwitchElectricity_Implementation() override;
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ImplementationBlueprintSwitch();

	virtual void SetHazardManager(class ATPP_ElectricityManager* ElectricityManager) override;

protected:
	UPROPERTY()
	class ATPP_ElectricityManager* ElectricityManagerRef;
};
