#pragma once

#include "CoreMinimal.h"
#include "TPP_HUD.h"
#include "AI/Enum/TPP_PongoEnum.h"
#include "Gameplay/GameModes/TPP_GameMode.h"
#include "TPP_HUDGame.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogPongoUI, Log, All);



USTRUCT(BlueprintType)
struct FIndicatorStatusData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FColor ColorToApplyAtMaterial;
	UPROPERTY(EditDefaultsOnly)
	float Frame;
	UPROPERTY(EditDefaultsOnly)
	float FrameAnimation;
	UPROPERTY(EditDefaultsOnly)
	float AnimationDuration;
	UPROPERTY(EditDefaultsOnly)
	float AnimationDelay;
	UPROPERTY()
	ETPP_PongoState CachedPongoState;

};

UENUM(BlueprintType)
enum class ECharacterTutorial : uint8
{

	Movement,
	Crouch,
	Jump,
	Push,
	Pet,
	ComeHere,
	Stop,
	Interact,
	GoThere,
	JaneInteract,
	None
};

UENUM(BlueprintType)
enum class EDrumUIAction : uint8
{
	ComeHere,
	Stop,
	Interact,
	GoThere
};
USTRUCT(BlueprintType)
struct FTutorialData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tutorial")
	TArray<class UTexture2D*> KeyboardImages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tutorial")
	TArray<class UTexture2D*> GamepadImages;
};

USTRUCT(BlueprintType)
struct FDeathText
{
	GENERATED_BODY()
	//For now will be a struct, later will implement animation if needed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DeathText")
	FText DeathMessage;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCutSceneSkipped);
/**
 * 
 */
UCLASS()
class TPP_01_API ATPP_HUDGame : public ATPP_HUD
{
	GENERATED_BODY()

	private:
	
	UFUNCTION()
	void OnLoadingScreenDeactivated();
	UFUNCTION()
	void OnDrumActivated();
	UFUNCTION()
	void OnDrumDeactivated();
	UFUNCTION()
	void OnComeHereActivated();
	UFUNCTION()
	void OnGoThereActivated();
	UFUNCTION()
	void OnGoThereCompleted();
	UFUNCTION()
	void OnInteractActivated();
	UFUNCTION()
	void OnStopActivated();
	UFUNCTION()
	void OnPauseMenuOpen();
	UFUNCTION()
    void OnPauseMenuClosed();
	UFUNCTION()
	void OnGameOverCalled(ECharacterDeath CharacterDeath,EDeathCause DeathCause);
	virtual void BeginPlay() override;
	// PongoIndicator
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UPROPERTY()
	TObjectPtr<class APlayerCameraManager> CameraManager;
	
	UPROPERTY()
	TObjectPtr<class ATPP_GameMode> GameModeRef;

	UPROPERTY()
	TObjectPtr<class UTPP_GameInstance> GameInstanceRef;
	UPROPERTY()
	TObjectPtr<class ATPP_PongoController> PongoController;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTPP_BaseWidget> GameOverClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTPP_LoadingScreen> LoadingScreenClass;
	UPROPERTY()
	TObjectPtr<class  UTPP_LoadingScreen> LoadingScreenPtr;

	FVector TargetLocation;
	
	bool IsActorInView(class APlayerController* PC, class AActor* Actor) const;
	
	FVector2D GetActorDirection(APlayerController* PC, AActor* Actor) const;
	
	float GetActorsDistance(class AActor* Actor1, class AActor* Actor2) const;
	
	float GetAngleToTargetCamera(APlayerController* PC, AActor* Target) const;
	
	FTimerHandle TimerBeginPongoIndicator;

	FVector PongoStartLocation;
	
	UPROPERTY(EditAnywhere)
	float PongoIndicatorDelay = 2.0f;
	UPROPERTY()
	float Tolerance = 50.f;
	UPROPERTY()
	bool bIsFirstRoom;
	UPROPERTY()
	bool bWasDrumInUse;
	
	UPROPERTY()
	TObjectPtr<class UTPP_PongoIndicator> PongoIndicator;
	
protected:
	
	UPROPERTY(EditAnywhere,Category="PongoIndicator")
	TSubclassOf<class UTPP_BaseWidget> PongoIndicatorClass;
	
	UFUNCTION()
	void ReadPongoStatus(ETPP_PongoState PongoState);
	
	UPROPERTY(EditAnywhere,Category = "PongoStatusIndicator") 
	TMap<ETPP_PongoState,FIndicatorStatusData> PongoStatusData;
	
	FIndicatorStatusData CurrentIndicatorStatusData;
	
	ETPP_PongoState SavedPongoState = ETPP_PongoState::Idle ;
	//EndPongoIndicator
	UPROPERTY()
	TObjectPtr<class UTPP_DrumManager> DrumManager;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTPP_DrumManager> DrumManagerClass;

	UFUNCTION()
	void SetPongoController();
	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tutorial")
	TMap<ECharacterTutorial,FTutorialData> MapTutorialData;
	
	UPROPERTY()
	FTimerHandle TimerHandle_RemoveDrum;
	UPROPERTY()
	FTimerHandle TimerHandle_ShowDrum;
	UPROPERTY(EditAnywhere,Category="LoadingScreen")
	bool bRemoveLoadingScreen;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TimerShowDrum")
	float TimerShowDrum = 1.f;
	
	UFUNCTION()
	void RemoveDrumWidget();

	public:
	FTutorialData* GetDataTutorial(ECharacterTutorial Tutorial);
	UFUNCTION(BlueprintCallable)
	void ShowDrum();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DeathSettingsText")
	TMap<EDeathCause,FDeathText> DeathTextMap;

	void DisableInputPlayer();
	void EnableInputPlayer();
	UFUNCTION()
	void OnLoadingScreenActivated();

	UPROPERTY()
	class UTPP_GameOverWidget* GameOverWidget;

	bool bHasPongoBeingPetOnce;

	UFUNCTION()
	void OnPetFinishedHUD();

	public:
		
	FORCEINLINE	bool GetHasPongoBeingPetOnce() const { return bHasPongoBeingPetOnce; }

	FOnCutSceneSkipped OnCutSceneSkipped;

	void DeactivateDrumCutscene();
};
