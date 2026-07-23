// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "TPP_BaseButton.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class TPP_01_API UTPP_BaseButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	virtual void ResetInputAction();
	

	UFUNCTION(BlueprintCallable)
	virtual void ForceRefreshStyle();
protected:
	
	UPROPERTY(EditAnywhere, Category="Button Settings")
	bool bAllowInputIcon = false;
	
	UPROPERTY(EditAnywhere, Category="Button Settings", meta=(InlineEditConditionToggle))
	uint8 bOverrideButtonText : 1;

	UPROPERTY(EditAnywhere, Category="Button Settings", meta=(editcondition="bOverrideButtonText" ))
	FText ButtonText;

	UPROPERTY()
	FDataTableRowHandle DefaultInputActionRow;
	
	UPROPERTY()
	FDataTableRowHandle NullInputActionRow;

	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void HandleFocusLost() override;
	virtual void NativeOnSelected(bool bBroadcast) override;
	virtual void NativeOnDeselected(bool bBroadcast) override;
	
	virtual void UpdateInputActionWidget() override;
	virtual void OnInputMethodChanged(ECommonInputType CurrentInputType) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void RefreshButtonText();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonText(const FText& InText);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonStyle();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonImage(const FSlateBrush& InBrush);
};

