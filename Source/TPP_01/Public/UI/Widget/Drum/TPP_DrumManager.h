// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/HUD/TPP_HUDGame.h"
#include "UI/Widget/TPP_BaseWidget.h"
#include "TPP_DrumManager.generated.h"

UENUM(BlueprintType)
enum class EDrumComponentState : uint8
{
	Pressed,
	Active,
	Released
};

USTRUCT(BlueprintType)
struct FDrumComponentData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FMargin WidgetMargin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drum|Layout")
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drum|Layout")
	TEnumAsByte<EVerticalAlignment> VerticalAlignment;
	UPROPERTY(EditAnywhere)
	TMap<EDrumComponentState,float> MapAnimation;
	UPROPERTY()
	TObjectPtr<class UTPP_DrumComponent> DrumComponent;
	UPROPERTY()
	FTimerHandle TimerHandleChangeState;
	UPROPERTY(EditAnywhere)
	float TimeToReset;
};
/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_DrumManager : public UTPP_BaseWidget
{
	GENERATED_BODY()
	
	EDrumUIAction DrumUIAction;
	UPROPERTY(EditAnywhere)
	TMap<EDrumUIAction, FDrumComponentData> DrumComponentMap;
	UPROPERTY(meta=(BindWidget))
	class UOverlay* DrumOverlay;
	UPROPERTY()
	TObjectPtr<class ATPP_Drum> Drum;
	protected:
	FTimerHandle DrumAnimTimer;
	UPROPERTY(EditAnywhere,Category="Animation")
	float AnimDuration = 0.5f;
	UPROPERTY()
	float ElapsedTime = 0.f;
	UPROPERTY()
	FVector2D StartPos;
	UPROPERTY()
	FVector2D TargetPos;
	void UpdateDrumAnimation();
	public:
	UFUNCTION()
	void PlayAnimationDrumIn();
	UFUNCTION()
	void PlayAnimationDrumOut();
	
	
	void InitializeDrum();
	void SetWidgetVisibility();
	
	
	bool bIsPlayingAnimationIn;
	bool bIsPlayingAnimationOut;
	protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> WidgetClass;
	
	float GetTextureToApply(FDrumComponentData* Data,EDrumComponentState ComponentState);
	public:
	void ExecuteHudCommandDrum(EDrumUIAction DrumAction,bool bCanActivate);
	void SaveDataClosedMenu();
	void LoadDataClosedMenu();
	FORCEINLINE TObjectPtr<class ATPP_Drum> GetDrum() const {return Drum;}
};
