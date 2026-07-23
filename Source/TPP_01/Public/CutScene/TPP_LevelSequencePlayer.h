#pragma once

#include "CoreMinimal.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "GameFramework/Actor.h"
#include "UI/CutScene/TPP_CutSceneWidget.h"
#include "TPP_LevelSequencePlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSequenceFinished);

UCLASS()
class TPP_01_API ATPP_LevelSequencePlayer : public AActor
{
	GENERATED_BODY()

public:
	ATPP_LevelSequencePlayer();

protected:
	UPROPERTY(Transient)
	TObjectPtr<ULevelSequencePlayer> SequencePlayer;
	
	UPROPERTY(BlueprintReadWrite, Category = "Sequence|PlayerController")
	TObjectPtr<APlayerController> PC;
	
	UPROPERTY(BlueprintReadWrite, Category = "Sequence|Widget")
	TObjectPtr<UTPP_CutSceneWidget> LevelSequenceWidget;

	UPROPERTY(BlueprintReadWrite, Category = "Sequence")
	TObjectPtr<ALevelSequenceActor> LevelSequenceActorRef;

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void OnSequenceFinished();

	void StopPlayerMovement() const;

public:
	UPROPERTY(BlueprintAssignable, Category = "Sequence")
	FOnSequenceFinished OnSequenceFinishedDelegate;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ALevelSequenceActor> SequenceActorRef;

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void PlaySequence(class ULevelSequence* Sequence, AActor* BindingActor = nullptr);

	UPROPERTY(BlueprintReadOnly, Category = "Sequence | Widget")
	TSubclassOf<UTPP_CutSceneWidget> LevelSequenceWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "Sequence | Pet")
	void PlayPetSequence();

	UPROPERTY(EditDefaultsOnly, Category = "Sequence | Pet")
	TSoftObjectPtr<ULevelSequence> PetSequenceSoft;
	
	
	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void StopSequence();

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void PauseSequence();

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	bool IsPlaying() const;
};
