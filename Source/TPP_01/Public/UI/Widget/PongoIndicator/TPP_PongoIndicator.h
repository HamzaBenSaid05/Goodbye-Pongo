#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/TPP_BaseWidget.h"
#include "AI/Enum/TPP_PongoEnum.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "TPP_PongoIndicator.generated.h"


UENUM(BlueprintType)
enum class ETPP_PongoAnimState : uint8
{
	OriginalToZero,        
	NewToOne,  
	NewToZero,
	OriginalToOne
};
/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_PongoIndicator : public UTPP_BaseWidget
{
	GENERATED_BODY()

protected:
	
	virtual void NativeConstruct() override;
	UPROPERTY(meta = (BindWidget))
	class UOverlay* OverlaySlot;
	UPROPERTY(meta = (BindWidget))
	class UImage* ImageIndicator;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterialImageIndicator;
	UPROPERTY(meta = (BindWidget))
	class UImage* Arrow;
	UPROPERTY(meta = (BindWidget))
	class UImage* PongoIcon;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterialPongoIcon;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> PongoIconOriginalMaterial;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> PongoIconAnimationMaterial;
	UPROPERTY(meta = (BindWidget))
	class UImage* IndicatorBackGround;
	UPROPERTY(meta = (BindWidget))
	class USizeBox* SizeBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Indicator")
	float MinScale = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Indicator")
	float MaxScale = 1.5f;
	UPROPERTY(EditAnywhere, Category = "Scalability")
	float ScalabilityTreshOld;
	UPROPERTY(EditAnywhere, Category = "Screen")
	float ScreenTreshOldMulty = 0.8f;
	UPROPERTY(EditAnywhere, Category = "Screen")
	float ScreenOffsetX = 50;
	UPROPERTY(EditAnywhere, Category = "Screen")
	float ScreenOffsetY = 80;
	UPROPERTY(EditAnywhere, Category = "Scalability")
	float Margin = 0.9f;
	UPROPERTY(EditAnywhere, Category = "AngleOffeset")
	float AngleOffeset;
	UPROPERTY()
	FVector2D FinalPos;
	UPROPERTY(EditAnywhere, Category = "AngleOffeset")
	float Radius = 40.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oscillation")
	float MaxRotationAngle = 25.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oscillation")
	float OscillationSpeed = 10.0f;
	UPROPERTY(EditAnywhere, Category = "KeyStrollTime")
	FName StrollTimeKey;
private:
	float RunningTime = 0.0f;
	void ChangePongoIconBaseMaterial(UMaterialInterface* NewBaseMaterial);
	ETPP_PongoAnimState CurrentAnimState = ETPP_PongoAnimState::OriginalToZero;
	FTimerHandle PongoAnimTimerHandle;

	float StateElapsedTime = 0.0f;
	float TargetDuration = 1.0f;
	float ElapsedTime = 0.0f;
	float CachedAnimDuration = 1.f;
	float CashedAnimFrame = 0.0f;
	ETPP_PongoState CashedPongoState;

public:
	UPROPERTY()
	int32 SideVectorX;
	void SetAngle(float Angle);
	void SetImageSide(FVector2D Direction);
	void EvaluateDistance(float Distance);
	void SetIndicatorColorAndFrame(struct FIndicatorStatusData Data);
	void PlayPongoIconAnimation(bool bIsMoving,float DeltaTime);
	void PlayFeedbackAnimation(ETPP_PongoState PongoState);
	void UpdateFeedbackAnimation();

public:
	FORCEINLINE FVector2D GetFinalPos() { return FinalPos; }
	FORCEINLINE FVector2D GetDirection() { return GetFinalPos().GetSafeNormal(); }
	FVector2D GetImageRatio();
	float GetAngle();
};
