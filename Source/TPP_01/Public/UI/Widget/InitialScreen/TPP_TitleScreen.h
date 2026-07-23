#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/TPP_BaseWidget.h"
#include "TPP_TitleScreen.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_TitleScreen : public UTPP_BaseWidget
{
	GENERATED_BODY()
	
	protected:
	UPROPERTY(EditAnywhere,Category= "WidgetClass")
	TArray<TSubclassOf<class UTPP_BaseWidget>> Widgets;

	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetSwitcher* Switcher;

	FTimerHandle TitleScreenTimerHandle;

	UPROPERTY(EditAnywhere,Category= "Delay")
	float DelaySeconds = 5;
	int CurrentScreenIndex = 0;
	UPROPERTY(BlueprintReadWrite)
	bool bCanChangeScreen = false;
protected:
	virtual void NativeOnActivated() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "Title Screen")
	void LaunchMainMenu();
	void SwitchControllerPage();
	
};
