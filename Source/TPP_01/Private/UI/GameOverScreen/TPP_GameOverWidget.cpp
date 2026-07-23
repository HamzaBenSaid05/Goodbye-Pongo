// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GameOverScreen/TPP_GameOverWidget.h"

#include "Components/TextBlock.h"

void UTPP_GameOverWidget::InizializeGameOverWidget(ECharacterDeath CharacterDeath, const FText& DeathMessage)
{
	
	PlayerBeginDeathMessage = FText::FromString(TEXT("Player {DeathMessage}"));
	PongoBeginDeathMessage = FText::FromString(TEXT("Pongo {DeathMessage}"));
	
	FFormatNamedArguments Args;
	Args.Add(TEXT("DeathMessage"), DeathMessage);

	FText FinalText;

	if (CharacterDeath == ECharacterDeath::PlayerDeath)
	{
		FinalText = FText::Format(PlayerBeginDeathMessage, Args);
	}
	else
	{
		FinalText = FText::Format(PongoBeginDeathMessage, Args);
	}
	
	DeathTextBlock->SetText(MoveTemp(FinalText));
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("WIDGET DEBUG: %s"), *FinalText.ToString()));
	}
	
}
