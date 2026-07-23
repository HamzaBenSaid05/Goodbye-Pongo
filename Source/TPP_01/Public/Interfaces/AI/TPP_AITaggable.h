#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "TPP_AITaggable.generated.h"

UINTERFACE(Blueprintable)
class UTPP_AITaggable : public UInterface
{
	GENERATED_BODY()
};

class TPP_01_API ITPP_AITaggable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AI|Tag")
	void AddGameplayTag(FGameplayTag Tag);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AI|Tag")
	void RemoveGameplayTag(FGameplayTag Tag);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AI|Tag")
	bool HasGameplayTag(FGameplayTag Tag);
};
