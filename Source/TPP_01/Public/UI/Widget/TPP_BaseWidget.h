// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "CommonUI/Private/Input/UIActionRouterTypes.h"
#include "Input/CommonUIInputTypes.h"
#include "TPP_BaseWidget.generated.h"

struct FUIInputConfig;

UENUM(BlueprintType)
enum class ETPPWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};
/**
 * 
 */


UCLASS()
class TPP_01_API UTPP_BaseWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

#if WITH_EDITOR
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, class IWidgetCompilerLog& CompileLog) const override;
#endif
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = Input)
	ETPPWidgetInputMode InputConfig = ETPPWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;

};
