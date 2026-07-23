// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/GameModes/TPP_GameMode.h"
#include "UI/Widget/TPP_BaseWidget.h"
#include "TPP_GameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_GameOverWidget : public UTPP_BaseWidget
{
	GENERATED_BODY()
	
	public:
	void InizializeGameOverWidget(ECharacterDeath CharacterDeath, const FText& DeathMessage);
	
protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DeathTextBlock;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DeathText")
	FText PlayerBeginDeathMessage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DeathText")
	FText PongoBeginDeathMessage;
};
