#pragma once

#include "CoreMinimal.h"
#include "AI/Enum/TPP_PongoEnum.h"
#include "Engine/DataAsset.h"
#include "TPP_FurTransitionSocketMap.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_FurTransitionSocketMap : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Key is the Pongo State, Value is the name of the Socket from where the transition should start"))
	TMap<ETPP_PongoState, FName> SocketMap;
};
