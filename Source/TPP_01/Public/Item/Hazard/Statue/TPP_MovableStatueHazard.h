#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Hazards/NeedPowerSwitch.h"
#include "Item/Hazard/Statue/TPP_BaseStatueHazard.h"
#include "Spline/TPP_Spline.h"
#include "TPP_MovableStatueHazard.generated.h"


UCLASS()
class TPP_01_API ATPP_MovableStatueHazard : public ATPP_BaseStatueHazard, public INeedPowerSwitch
{
	GENERATED_BODY()

public:
	ATPP_MovableStatueHazard();
	//UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Movement", meta = (DisplayPriority = "1"))
	//TArray<FSplinePointParameters> SplinePointParameters;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta = (DisplayPriority = "1"))
	class ATPP_Spline* SplineComp;

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	// INeedPowerMethods
	virtual void SwitchPower_Implementation() override;
	// INeedPowerEndMethods
	UPROPERTY()
	bool bIsFirstSwitch = false;
	FSplinePointParameters SplineParam;
	UPROPERTY()
	FVector NextSplinePoint = FVector::ZeroVector;
	UPROPERTY()
	float NextSplineLenght;
	UPROPERTY()
	float CurrentIndex;
	UPROPERTY()
	float ElapseTime;

	void UpdateSplinePointParameters(int IndexPoint);
	UPROPERTY()
	float DistanceAlongSpline = 0.f;
	UPROPERTY()
	float SplineLength = 0.f;
	UFUNCTION(BlueprintCallable, Category="Movement")
	void ChangeSpline(ATPP_Spline* NewSpline);
	//====Audio====
	UPROPERTY()
	UAudioComponent* AudioCompMove;
	//====Audio====

private:
	void MoveStatueHazard(float DeltaTime);

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Movement")
	void OnStatueFinishMoving();
};
