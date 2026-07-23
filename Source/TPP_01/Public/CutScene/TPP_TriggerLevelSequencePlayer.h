#pragma once

#include "CoreMinimal.h"
#include "TPP_LevelSequencePlayer.h"
#include "Engine/TriggerBox.h"
#include "TPP_TriggerLevelSequencePlayer.generated.h"

UCLASS()
class TPP_01_API ATPP_TriggerLevelSequencePlayer : public ATriggerBox
{
	GENERATED_BODY()

public:
	ATPP_TriggerLevelSequencePlayer();
	UPROPERTY()
	TObjectPtr<class ATPP_LevelSequencePlayer> LevelSequencePlayer; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	TSoftObjectPtr<ULevelSequence> LevelSequenceSoft;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "TriggerLevelSequence")
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
											  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "Sequence")
	ULevelSequence* GetSequence(TSoftObjectPtr<ULevelSequence> LevelSequenceSoftPtr) const
	{
		ULevelSequence* LevelSequence = LevelSequenceSoftPtr.LoadSynchronous();
		return LevelSequence ? LevelSequence : nullptr;
	} 
};
