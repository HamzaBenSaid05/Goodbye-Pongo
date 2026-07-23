#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "TPP_EscapeFearRange.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPP_01_API UTPP_EscapeFearRange : public USphereComponent
{
	GENERATED_BODY()
public:
	UTPP_EscapeFearRange();

protected:
	virtual void OnRegister() override;
};
