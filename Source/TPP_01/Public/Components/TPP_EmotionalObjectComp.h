#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Characters/Pongo/TPP_Pongo.h"
#include "TPP_EmotionalObjectComp.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPP_01_API UTPP_EmotionalObjectComp : public USphereComponent
{
	GENERATED_BODY()

public:
	UTPP_EmotionalObjectComp();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRegister() override;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void CheckPongoVisibility();
	void SendEmotionalStateEvent(ETPP_PongoState NewEmotionalState);
	void ResetEmotionalState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emotional")
	ETPP_PongoState EmotionalState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emotional")
	FName HeadBoneName = "head";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interact",
		meta=(AllowPrivateAccess="true", EditCondition="EmotionalState == ETPP_PongoState::Curiosity", EditConditionHides))
	bool bAutoDestroy = false;
	//====Audio====
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTPP_SoundComponent> SoundComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Tag")
	FGameplayTag TagEmotionalState;
	//====Audio====
	
private:
	TWeakObjectPtr<ATPP_Pongo> PongoActor;
	TWeakObjectPtr<APawn> PlayerPawn;
	FTimerHandle PongoTimerHandle;
	TEnumAsByte<ECollisionChannel> PongoChannel;
	bool bEventSent = false;

public:
	FORCEINLINE ETPP_PongoState GetEmotionalState() const { return EmotionalState; }
	FORCEINLINE bool  GetbIsAutoDestroy() const { return bAutoDestroy; }
};