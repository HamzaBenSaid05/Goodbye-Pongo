#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "TPP_EnvQueryTest_NotInEmotionRanges.generated.h"

UCLASS()
class TPP_01_API UTPP_EnvQueryTest_NotInEmotionRanges : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UTPP_EnvQueryTest_NotInEmotionRanges();

	UPROPERTY(EditDefaultsOnly, Category = "EQS")
	TSubclassOf<UEnvQueryContext> Context;

	UPROPERTY(EditDefaultsOnly, Category="Emotion")
	float ExtraBuffer = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Emotion")
	bool bUse2D = false;
	
	// Distance from the sphere border that yields maximum score
	UPROPERTY(EditDefaultsOnly, Category="Emotion")
	float MaxGoodDistanceFromBoundary = 1000.f;
	
protected:
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};
