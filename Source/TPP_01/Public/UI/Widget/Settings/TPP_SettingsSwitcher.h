// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/TPP_BaseButton.h"
#include "TPP_SettingsSwitcher.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_SettingsSwitcher : public UTPP_BaseButton
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;
	TArray<FUIActionBindingHandle> BindingHandles;
	virtual void HandleFocusReceived() override;
	virtual void HandleFocusLost() override;
	virtual void NativeOnInitialized() override;
	UPROPERTY()
	TObjectPtr<class UTPP_GameInstance> GameInstanceRef;
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* Switcher;

	int32 CurrentSettingsIndex = 2;
	
	void ChangeGraphicSettingsUp();
	void ChangeGraphicSettingsDown();

	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	FDataTableRowHandle NextTabActionRow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	FDataTableRowHandle PrevTabActionRow;
};
