#include "CutScene/TPP_LevelSequencePlayer.h"

#include "LevelSequencePlayer.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/HUD/TPP_HUDGame.h"

ATPP_LevelSequencePlayer::ATPP_LevelSequencePlayer()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATPP_LevelSequencePlayer::OnSequenceFinished()
{
	if (PC == nullptr)
		PC = GetWorld()->GetFirstPlayerController();
	
	if (LevelSequenceWidget)
	{
		LevelSequenceWidget->DeactivateCutSceneWidget();
		if (ATPP_HUDGame* GameHud = Cast<ATPP_HUDGame>(PC->GetHUD()))
		{
			GameHud->PopWidgetFromLayer(
				FGameplayTag::RequestGameplayTag(FName("UI.Layer.Menu")),
				LevelSequenceWidget
			);

			if (GameHud)
			{
				GameHud->OnCutSceneSkipped.Broadcast();
			}
		}
	}
	
	PC->SetInputMode(FInputModeGameOnly());
	PC->bShowMouseCursor = false;
	PC->StopMovement();
	PC->SetIgnoreMoveInput(false);
	PC->SetIgnoreLookInput(false);
	OnSequenceFinishedDelegate.Broadcast();
}

void ATPP_LevelSequencePlayer::StopPlayerMovement() const
{
	if (!PC) return;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return;

	if (UCharacterMovementComponent* MoveComp =
		Pawn->FindComponentByClass<UCharacterMovementComponent>())
	{
		MoveComp->StopMovementImmediately();
	}
}

void ATPP_LevelSequencePlayer::PlaySequence(ULevelSequence* Sequence, AActor* BindingActor)
{
	if (!Sequence || !GetWorld() || !LevelSequenceWidgetClass) return;

	PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;
	
	ATPP_HUDGame* GameHud = Cast<ATPP_HUDGame>(PC->GetHUD());

	ALevelSequenceActor* SequenceActor = nullptr;
	FMovieSceneSequencePlaybackSettings PlaybackSettings;
	
	SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(),
		Sequence,
		PlaybackSettings,
		SequenceActor
	);
	
	SequenceActorRef = SequenceActor;
	if (SequenceActorRef)
	{
		SequenceActorRef->SetOwner(this);
	}
	
	if (!SequencePlayer) return;
	
	if (GameHud)
	{
		GameHud->DeactivateDrumCutscene();
		LevelSequenceWidget = Cast<UTPP_CutSceneWidget>(
			GameHud->PushWidgetToLayer(
				FGameplayTag::RequestGameplayTag(FName("UI.Layer.Menu")),
				LevelSequenceWidgetClass
			)
		);
	}
	
	if (!LevelSequenceWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("PlaySequence: LevelSequenceWidget is null! "
			   "GameHud=%s, PushWidgetToLayer returned null or Cast failed."),
			   GameHud ? TEXT("valid") : TEXT("NULL"));
		SequencePlayer->Stop();
		return;
	}

	LevelSequenceWidget->LevelSequencePlayer = this;

	PC->SetInputMode(FInputModeGameAndUI());
	PC->bShowMouseCursor = true;
	
	PC->SetIgnoreMoveInput(true);
	PC->SetIgnoreLookInput(true);

	StopPlayerMovement();
	
	SequencePlayer->OnFinished.AddDynamic(this, &ATPP_LevelSequencePlayer::OnSequenceFinished);
	SequencePlayer->Play();
}

void ATPP_LevelSequencePlayer::PlayPetSequence()
{
	if (ULevelSequence* LevelSequence = PetSequenceSoft.LoadSynchronous())
	{
		PlaySequence(LevelSequence);
	}
}

void ATPP_LevelSequencePlayer::StopSequence()
{
	if (SequencePlayer && SequencePlayer->IsPlaying())
	{
		SequencePlayer->Stop();
		if (LevelSequenceWidget)
		{
			LevelSequenceWidget->DeactivateCutSceneWidget();
			if (ATPP_HUDGame* GameHud = Cast<ATPP_HUDGame>(PC->GetHUD()))
			{
				GameHud->PopWidgetFromLayer(
					FGameplayTag::RequestGameplayTag(FName("UI.Layer.Menu")),
					LevelSequenceWidget
				);
				
				if (GameHud)
				{
					GameHud->OnCutSceneSkipped.Broadcast();
				}
			}
		}
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);
		StopPlayerMovement();
		OnSequenceFinishedDelegate.Broadcast();
	}
}

void ATPP_LevelSequencePlayer::PauseSequence()
{
	if (SequencePlayer && SequencePlayer->IsPlaying())
	{
		SequencePlayer->Pause();
		PC = GetWorld()->GetFirstPlayerController();
		if (LevelSequenceWidget)
		{
			LevelSequenceWidget->DeactivateCutSceneWidget();
			if (ATPP_HUDGame* GameHud = Cast<ATPP_HUDGame>(PC->GetHUD()))
			{
				GameHud->PopWidgetFromLayer(
					FGameplayTag::RequestGameplayTag(FName("UI.Layer.Menu")),
					LevelSequenceWidget
				);
				if (GameHud)
				{
					GameHud->OnCutSceneSkipped.Broadcast();
				}
			}
		}
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);
		StopPlayerMovement();
		OnSequenceFinishedDelegate.Broadcast();
	}
}

bool ATPP_LevelSequencePlayer::IsPlaying() const
{
	return SequencePlayer && SequencePlayer->IsPlaying();
}
