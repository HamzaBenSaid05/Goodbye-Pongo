// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "UI/Widget/TPP_BaseWidget.h"
#include "Engine/DataTable.h"
#include "Input/CommonUIInputTypes.h"
#include "Input/UIActionBindingHandle.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "TPP_SettingsManagerMenu.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_SettingsManagerMenu : public UTPP_BaseWidget
{
	GENERATED_BODY()
	

public:	
	UFUNCTION(BlueprintCallable)
	void OnChangeSettinAtIndex(int32 Index);
	
protected:
	
	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetSwitcher* Switcher;

	//virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
	UPROPERTY(BlueprintReadWrite,meta = (BindWidget))
	class UTPP_BaseWidget* BaseSettingsWidget;
	UPROPERTY(BlueprintReadWrite,meta = (BindWidget))
	class UTPP_BaseWidget* SoundSettingsWidget;
	UPROPERTY(BlueprintReadWrite,meta = (BindWidget))
	class UTPP_BaseWidget* ControlSettingsWidget;
	
	virtual void NativeConstruct() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	UFUNCTION(BlueprintNativeEvent)
	void HandleNextTab();
	UFUNCTION(BlueprintNativeEvent)
	void HandlePreviousTab();
	void ChangeSettingIndex(int32 Direction);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	FDataTableRowHandle NextTabActionRow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	FDataTableRowHandle PrevTabActionRow;
	int32 CurrentSettingIndex = 0;
	UPROPERTY(VisibleDefaultsOnly)
	int32 MaxCount = 2;
private:
	TArray<FUIActionBindingHandle> BindingHandles;
	
	
	
};
