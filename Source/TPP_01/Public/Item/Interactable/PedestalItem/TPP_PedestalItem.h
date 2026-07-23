#pragma once

#include "CoreMinimal.h"
#include "TPP_PedestalItemBase.h"
#include "Interfaces/Hazards/IPowerSwitch.h"
#include "TPP_PedestalItem.generated.h"

UCLASS()
class TPP_01_API ATPP_PedestalItem : public ATPP_PedestalItemBase,public IIPowerSwitch
{
	GENERATED_BODY()

public:
	ATPP_PedestalItem();
	UFUNCTION()
	virtual void SwitchElectricity_Implementation() override;
	UFUNCTION()
	virtual void SetHazardManager(class ATPP_ElectricityManager* ElectricityManager) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
protected:
	UPROPERTY()
	class ATPP_ElectricityManager* ElectricityManagerRef;
	virtual void EnableObject() override;
	virtual void DisableObject() override;
	virtual void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
