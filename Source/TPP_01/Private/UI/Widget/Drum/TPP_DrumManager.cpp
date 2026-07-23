// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/Drum/TPP_DrumManager.h"

#include "Characters/Player/TPP_PlayerCharacter.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Components/Overlay.h"
#include "Drum/TPP_Drum.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widget/Drum/TPP_DrumComponent.h"

void UTPP_DrumManager::UpdateDrumAnimation()
{
	if (!DrumOverlay) return;

	
	ElapsedTime += GetWorld()->GetDeltaSeconds();

	float Alpha = FMath::Clamp(ElapsedTime / AnimDuration, 0.f, 1.f);

	FVector2D CurrentPos = FMath::Lerp(StartPos, TargetPos, Alpha);

	DrumOverlay->SetRenderTranslation(CurrentPos);

	if (Alpha >= 1.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(DrumAnimTimer);
		bIsPlayingAnimationIn = false;
		bIsPlayingAnimationOut = false;
	}
}

void UTPP_DrumManager::PlayAnimationDrumIn()
{
	if (!DrumOverlay) return;

	GetWorld()->GetTimerManager().ClearTimer(DrumAnimTimer);

	StartPos =  DrumOverlay->GetRenderTransform().Translation;
	TargetPos = FVector2D(0.f, 0);
	
	ElapsedTime = 0.f;

	GetWorld()->GetTimerManager().SetTimer(
		DrumAnimTimer,
		this,
		&UTPP_DrumManager::UpdateDrumAnimation,
		GetWorld()->GetDeltaSeconds(),
		true
	);
	bIsPlayingAnimationOut = false;
	bIsPlayingAnimationIn = true;
}

void UTPP_DrumManager::PlayAnimationDrumOut()
{
	if (!DrumOverlay) return;
	
	GetWorld()->GetTimerManager().ClearTimer(DrumAnimTimer);

	StartPos = DrumOverlay->GetRenderTransform().Translation;
	TargetPos = FVector2D(0.f, 400.f);
	
	ElapsedTime = 0.f;

	GetWorld()->GetTimerManager().SetTimer(
		DrumAnimTimer,
		this,
		&UTPP_DrumManager::UpdateDrumAnimation,
		GetWorld()->GetDeltaSeconds(),
		true
	);
	
	bIsPlayingAnimationOut = true;
	bIsPlayingAnimationIn = false;
}


void UTPP_DrumManager::InitializeDrum()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PC)
	{
		ATPP_PlayerCharacter* PlayerCharacter = Cast<ATPP_PlayerCharacter>(PC->GetPawn());
		if (PlayerCharacter)
		{
			Drum = PlayerCharacter->GetDrum();
		}
	}
	
	if (Drum)
	{
		for (TPair<EDrumUIAction, FDrumComponentData>& DrumData : DrumComponentMap)
		{
			FDrumComponentData& WidgetData = DrumData.Value;
			if (WidgetData.DrumComponent)
			{
				continue;
			}
			if (WidgetClass)
			{
				WidgetData.DrumComponent = CreateWidget<UTPP_DrumComponent>(this, WidgetClass);
				if (WidgetData.DrumComponent)
				{
					WidgetData.DrumComponent->SetPadding(WidgetData.WidgetMargin);
					if (DrumOverlay)
					{
						UOverlaySlot* SlotImage = DrumOverlay->AddChildToOverlay(WidgetData.DrumComponent);
						if (SlotImage)
						{
							SlotImage->SetHorizontalAlignment(WidgetData.HorizontalAlignment);
							SlotImage->SetVerticalAlignment(WidgetData.VerticalAlignment);
						}
					}
				}
			}
		}
	}
	
	SetWidgetVisibility();
	PlayAnimationDrumIn();

}

void UTPP_DrumManager::SetWidgetVisibility()
{
	if (Drum)
	{
		FDrumComponentData* GoThereData= DrumComponentMap.Find(EDrumUIAction::GoThere);
		FDrumComponentData* InteractData= DrumComponentMap.Find(EDrumUIAction::Interact);
		FDrumComponentData* ComeHereData= DrumComponentMap.Find(EDrumUIAction::ComeHere);
		FDrumComponentData* StopData= DrumComponentMap.Find(EDrumUIAction::Stop);
	
		if (GoThereData)
		{
			if (Drum->bGoThereActive)
			{
				if (GoThereData->DrumComponent)
				{
					GoThereData->DrumComponent->SetFrameMaterialFromSpreadSheet(GetTextureToApply(GoThereData,EDrumComponentState::Active));
				}
			}
			else
			{
				if (GoThereData->DrumComponent)
					GoThereData->DrumComponent->SetFrameMaterialFromSpreadSheet(0);
				
			}
		}
	
		if (InteractData)
		{
			if (Drum->bInteractActive)
			{
				if (InteractData->DrumComponent)
				{
					InteractData->DrumComponent->SetFrameMaterialFromSpreadSheet(GetTextureToApply(InteractData,EDrumComponentState::Active));
				}
			}
			else
			{
				if (InteractData->DrumComponent)
					InteractData->DrumComponent->SetFrameMaterialFromSpreadSheet(0);
			}
		}
			
		if (ComeHereData)
		{
			if (ComeHereData->DrumComponent)
			{
				ComeHereData->DrumComponent->SetFrameMaterialFromSpreadSheet(GetTextureToApply(ComeHereData,EDrumComponentState::Active));
			}
		}
	
		if (StopData)
		{
			if (StopData->DrumComponent)
			{
					StopData->DrumComponent->SetFrameMaterialFromSpreadSheet(GetTextureToApply(StopData,EDrumComponentState::Active));
			}
		}
	}
}

void UTPP_DrumManager::SaveDataClosedMenu()
{
	StartPos = DrumOverlay->GetRenderTransform().Translation;
}

void UTPP_DrumManager::LoadDataClosedMenu()
{
	DrumOverlay->SetRenderTranslation(StartPos);
	
	if (bIsPlayingAnimationIn)
		PlayAnimationDrumIn();
	if (bIsPlayingAnimationOut)
		PlayAnimationDrumOut();
}

float UTPP_DrumManager::GetTextureToApply(FDrumComponentData* Data,EDrumComponentState ComponentState)
{
	float* ValueFound= Data->MapAnimation.Find(ComponentState);
					
	if (ValueFound != nullptr)
		return *ValueFound;
	
	return 0;
}

void UTPP_DrumManager::ExecuteHudCommandDrum(EDrumUIAction DrumAction,bool bCanActivate)
{
	if (!bCanActivate)
		return;
	
	FDrumComponentData* WidgetData = DrumComponentMap.Find(DrumAction);
	
	if (!WidgetData || !WidgetData->DrumComponent)
	{
		return;
	}
	
	WidgetData->DrumComponent->SetFrameMaterialFromSpreadSheet(GetTextureToApply(WidgetData,EDrumComponentState::Pressed));
	
	UWorld* World = GetWorld();
	if (World)
	{
		if (World->GetTimerManager().IsTimerActive(WidgetData->TimerHandleChangeState))
		{
			return; 
		}
		
		World->GetTimerManager().SetTimer(
			WidgetData->TimerHandleChangeState,
			this,
			&UTPP_DrumManager::SetWidgetVisibility, 
			WidgetData->TimeToReset,                                 
			false                                 
		);
	}
}
