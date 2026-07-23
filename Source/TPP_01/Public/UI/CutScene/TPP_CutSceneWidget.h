#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/TPP_BaseWidget.h"
#include "TPP_CutSceneWidget.generated.h"

UCLASS()
class TPP_01_API UTPP_CutSceneWidget : public UTPP_BaseWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene")
	TObjectPtr<class ATPP_LevelSequencePlayer> LevelSequencePlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cutscene")
	TObjectPtr<class UTPP_Comic_Base> ComicPlayed;

	UFUNCTION(BlueprintCallable, Category = "Cutscene")
	void InizializeCutSceneWidget(TSubclassOf<class UTPP_Comic_Base> ComicToPlay);

	UFUNCTION(BlueprintCallable, Category = "Cutscene")
	void DeactivateCutSceneWidget();

	UFUNCTION(BlueprintCallable)
	void SkipCutScene();

	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:

	UPROPERTY(meta = (BindWidget))
	class UCommonActionWidget* ActionWidget;

	UPROPERTY(meta = (BindWidget))
	class UImage* ProgressImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene|Hold Progress")
	FName MaterialParameterName = TEXT("Percentage");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene|Hold Progress")
	float MaxHoldTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene|Hold Progress")
	float ReleaseTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene|Visibility")
	float InactivityHideTime = 3.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene|Visibility")
	float HoldStartDelay = 0.15f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene|Visibility")
	float FadeDuration = 0.15f;
private:
	
	void StartHoldProgress();
	void StartReleaseProgress();
	void AdvanceHold();
	void AdvanceRelease();
	void UpdateMaterialPercentage(float NewPercentage);

	bool bIsHoldingKey = false;
	float CurrentPercentage = 0.0f;
	const float TimerFrequency = 0.01f;

	FTimerHandle HoldTimerHandle;
	FTimerHandle ReleaseTimerHandle;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	FTimerHandle InactivityTimerHandle;
	FTimerHandle DelayHoldTimerHandle;

	void ShowActionPrompt();
	void HideActionPrompt();

	FTimerHandle FadeTimerHandle;

	float TargetOpacity = 0.0f;
	float CurrentOpacity = 0.0f;

	void AdvanceFade();
};
