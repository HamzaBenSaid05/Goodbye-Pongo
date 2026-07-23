#pragma once

#include "CoreMinimal.h"
#include "LevelSequenceDirector.h"
#include "TPP_LevelSequenceDirector.generated.h"

UCLASS()
class TPP_01_API UTPP_LevelSequenceDirector : public ULevelSequenceDirector
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Sequence")
	TObjectPtr<class ATPP_LevelSequencePlayer> LevelSequencePlayerRef;
};
