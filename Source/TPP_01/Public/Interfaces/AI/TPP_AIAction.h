#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TPP_AIAction.generated.h"

UINTERFACE(Blueprintable)
class UTPP_AIAction : public UInterface
{
	GENERATED_BODY()
};

class TPP_01_API ITPP_AIAction
{
	GENERATED_BODY()

public:
	// Called when agent is on smart nav link
	virtual void Jump(const FVector& EndPos, const float& DestinationZOffset) = 0;

	// Called when agent is moving and need to change speed
	virtual void SetSpeedBasedOnState(uint8 State) = 0;

	// Called when agent reset color to default
	virtual void ResetColor() = 0;
	
	// Called when agent whant to change color
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AI|Collision")
	void SetColor(FLinearColor NewColor);

	// Called to toggle player collision
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AI|Collision")
	void TogglePlayerCollision(bool bEnable);
};
