// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SpecialButtons/TPP_BaseButtonSlider.h"
#include "Components/Slider.h"

bool UTPP_BaseButtonSlider::bLastInputWasGamepad = false;

FReply UTPP_BaseButtonSlider::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
    if (bLastInputWasGamepad && AnalogSlider)
    {
        return FReply::Handled().SetUserFocus(AnalogSlider->TakeWidget(), InFocusEvent.GetCause());
    }
    
    return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

FReply UTPP_BaseButtonSlider::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    const FKey Key = InKeyEvent.GetKey();
    const bool bIsGamepadKey = Key.IsGamepadKey();
    
    if (bLastInputWasGamepad != bIsGamepadKey)
    {
        bLastInputWasGamepad = bIsGamepadKey;
        
        if (bIsGamepadKey && AnalogSlider)
        {
            AnalogSlider->SetKeyboardFocus();
        }
       
        else if (!bIsGamepadKey && AnalogSlider && AnalogSlider->HasKeyboardFocus())
        {
            SetKeyboardFocus();
        }
    }

  

    if (Key == EKeys::Right || Key == EKeys::Gamepad_DPad_Right || Key == EKeys::Gamepad_LeftStick_Right)
    {
       AdjustSliderValue(1.0f);
       return FReply::Handled(); 
    }
    
    if (Key == EKeys::Left || Key == EKeys::Gamepad_DPad_Left || Key == EKeys::Gamepad_LeftStick_Left)
    {
       AdjustSliderValue(-1.0f); 
       return FReply::Handled(); 
    }
    
    return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UTPP_BaseButtonSlider::AdjustSliderValue(float StepDirection)
{
    if (!AnalogSlider) return;
    
    float CurrentValue = AnalogSlider->GetValue();
    float NewValue = CurrentValue + (SliderStepSize * StepDirection);

    NewValue = FMath::Clamp(NewValue, 0.0f, 1.0f);

    AnalogSlider->SetValue(NewValue);
    AnalogSlider->OnValueChanged.Broadcast(NewValue);
}
