// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CutScene/SkipButton/TPP_ButtonSkip.h"

#include "TimerManager.h"
#include "UI/HUD/TPP_HUDGame.h"
#include <Kismet/GameplayStatics.h>
#include "CommonActionWidget.h"

void UTPP_ButtonSkip::StartHoldProgress()
{
	
	GetWorld()->GetTimerManager().ClearTimer(ReleaseTimerHandle);

	
	if (CurrentPercentage >= 1.0f) return;

	GetWorld()->GetTimerManager().SetTimer(HoldTimerHandle, this, &UTPP_ButtonSkip::AdvanceHold, TimerFrequency, true);
}

void UTPP_ButtonSkip::StartReleaseProgress()
{
	
	GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);

	if (CurrentPercentage <= 0.0f) return;

	GetWorld()->GetTimerManager().SetTimer(ReleaseTimerHandle, this, &UTPP_ButtonSkip::AdvanceRelease, TimerFrequency, true);
}

void UTPP_ButtonSkip::AdvanceHold()
{
	if (MaxHoldTime > 0.0f)
	{
		
		CurrentPercentage += (TimerFrequency / MaxHoldTime);
		CurrentPercentage = FMath::Clamp(CurrentPercentage, 0.0f, 1.0f);

		UpdateMaterialPercentage(CurrentPercentage);

	
		if (CurrentPercentage >= 1.0f)
		{
			GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
			ATPP_HUDGame* Hud = Cast<ATPP_HUDGame>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());

			if (Hud)
			{
				Hud->OnSkipCutScene.Broadcast();
			}
		}
	}
	else
	{
		CurrentPercentage = 1.0f;
		UpdateMaterialPercentage(CurrentPercentage);
		GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
	}
}

void UTPP_ButtonSkip::AdvanceRelease()
{
	if (ReleaseTime > 0.0f)
	{

		CurrentPercentage -= (TimerFrequency / ReleaseTime);
		CurrentPercentage = FMath::Clamp(CurrentPercentage, 0.0f, 1.0f);

		UpdateMaterialPercentage(CurrentPercentage);


		if (CurrentPercentage <= 0.0f)
		{
			GetWorld()->GetTimerManager().ClearTimer(ReleaseTimerHandle);
		}
	}
	else
	{
		CurrentPercentage = 0.0f;
		UpdateMaterialPercentage(CurrentPercentage);
		GetWorld()->GetTimerManager().ClearTimer(ReleaseTimerHandle);
	}
}

void UTPP_ButtonSkip::UpdateMaterialPercentage(float NewPercentage)
{
	if (!ActionWidget) return;

	
	if (!DynamicMaterial)
	{
	
		UObject* BrushResource = ActionWidget->ProgressMaterialBrush.GetResourceObject();

		if (BrushResource)
		{
			
			UMaterialInterface* BaseMaterial = Cast<UMaterialInterface>(BrushResource);

			if (BaseMaterial)
			{
				
				DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);

				if (DynamicMaterial)
				{
					
					ActionWidget->ProgressMaterialBrush.SetResourceObject(DynamicMaterial);
				}
			}
		}
	}


	if (DynamicMaterial)
	{
		
		FName ParamName = ActionWidget->ProgressMaterialParam.IsNone() ? TEXT("Percentage") : ActionWidget->ProgressMaterialParam;

		DynamicMaterial->SetScalarParameterValue(ParamName, NewPercentage);

		
		ActionWidget->InvalidateLayoutAndVolatility();
	}
}