#pragma once

#include "CoreMinimal.h"
#include "Drum/TPP_DrumEnums.h"
#include "Engine/DataAsset.h"
#include "TPP_DrumCommandsColors.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_DrumCommandsColors : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<ETPP_DrumCommands, FLinearColor> CommandColors;
};
