#pragma once

#include "CoreMinimal.h"
#include "AI/Enum/TPP_PongoEnum.h"
#include "Engine/DataAsset.h"
#include "TPP_PongoReactions.generated.h"

UCLASS(BlueprintType)
class TPP_01_API UTPP_PongoReactions : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<ETPP_PongoReactions, int>  Reaction;
};
