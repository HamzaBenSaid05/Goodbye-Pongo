#include "UI/HUD/TPP_HUDGame.h"

#include "Characters/Pongo/TPP_Pongo.h"
#include "Gameplay/GameModes/TPP_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Engine.h"
#include "SceneView.h"
#include "GameplayTagContainer.h"
#include "UI/Widget/PongoIndicator/TPP_PongoIndicator.h"
#include "AI/AIController/TPP_PongoController.h"
#include "Drum/TPP_Drum.h"
#include "Gameplay/GameInstances/TPP_GameInstance.h"
#include "Player/TPP_PlayerController.h"
#include "UI/GameOverScreen/TPP_GameOverWidget.h"
#include "UI/Widget/Drum/TPP_DrumManager.h"
#include "UI/Widget/LoadingScreen/TPP_LoadingScreen.h"


DEFINE_LOG_CATEGORY(LogPongoUI)
#define PONGO_LOG(Verbosity, Format, ...) \
UE_LOG(LogPongoUI, Verbosity, TEXT("[%s]: " Format), *FString(__FUNCTION__), ##__VA_ARGS__)

#define PONGO_SCREEN(Color, Format, ...) \
if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, Color, FString::Printf(TEXT("[%s]: " Format), *FString(__FUNCTION__), ##__VA_ARGS__))

void ATPP_HUDGame::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
		CameraManager = PC->PlayerCameraManager;
	
	GameModeRef = Cast<ATPP_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameModeRef)
	{
		GameModeRef->OnGameOver.AddDynamic(this,&ATPP_HUDGame::OnGameOverCalled);
		GameModeRef->OnActivateLoadingScreen.AddDynamic(this,&ATPP_HUDGame::OnLoadingScreenActivated);
		GameModeRef->OnDeactivateLoadingScreen.AddDynamic(this,&ATPP_HUDGame::OnLoadingScreenDeactivated);
	}


	GameInstanceRef = Cast<UTPP_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	
	if (GameInstanceRef) {
		GameInstanceRef->SetHasGameBeenOpened(true);
	}
	
	OnLoadingScreenActivated();
	
	ATPP_PlayerController* PlayerControllerRef = Cast<ATPP_PlayerController>(PC);
	if (PlayerControllerRef)
	{
		PlayerControllerRef->OnDrumActivated.AddDynamic(this, &ATPP_HUDGame::OnDrumActivated);
		PlayerControllerRef->OnDrumDeactivated.AddDynamic(this, &ATPP_HUDGame::OnDrumDeactivated);
		PlayerControllerRef->OnComeHereActivated.AddDynamic(this, &ATPP_HUDGame::OnComeHereActivated);
		PlayerControllerRef->OnGoThereActivated.AddDynamic(this, &ATPP_HUDGame::OnGoThereActivated);
		PlayerControllerRef->OnGoThereCompleted.AddDynamic(this, &ATPP_HUDGame::OnGoThereCompleted);
		PlayerControllerRef->OnInteractActivated.AddDynamic(this, &ATPP_HUDGame::OnInteractActivated);
		PlayerControllerRef->OnStopActivated.AddDynamic(this,&ATPP_HUDGame::OnStopActivated);
		PlayerControllerRef->OnPauseMenuOpen.AddDynamic(this,&ATPP_HUDGame::OnPauseMenuOpen);
		PlayerControllerRef->OnPauseMenuClosed.AddDynamic(this,&ATPP_HUDGame::OnPauseMenuClosed);
	}

	if (const UTPP_GameInstance* GI = Cast<UTPP_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
		bIsFirstRoom = GI->GetRoomIndexToLoad() <= 0;
	SetPongoController();

	
}

void ATPP_HUDGame::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PongoController)
	{
		PongoController->OnPongoChangedState.RemoveDynamic(
			this,
			&ThisClass::ReadPongoStatus
		);
	}

	Super::EndPlay(EndPlayReason);
}

void ATPP_HUDGame::SetPongoController()
{
	if (!GameModeRef->GetPongoRef()->GetController()) return;
	PongoController = Cast<ATPP_PongoController>(GameModeRef->GetPongoRef()->GetController());
	PongoStartLocation = GameModeRef->GetPongoRef()->GetActorLocation();
	PongoController->OnPongoChangedState.AddDynamic(this,&ThisClass::ReadPongoStatus);
	SavedPongoState = PongoController->GetPongoState();
	if (GameModeRef->GetPongoRef()) {
		GameModeRef->GetPongoRef()->OnPetFinished.AddDynamic(this, &ATPP_HUDGame::OnPetFinishedHUD);
	}
}

void ATPP_HUDGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		if(PongoIndicator)
		{
			PopWidgetFromLayer(
				FGameplayTag::RequestGameplayTag(FName("UI.Layer.Game")),
				PongoIndicator
			);
			PongoIndicator = nullptr;
		}
		return;
	}
	
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC || !GameModeRef || !GameModeRef->GetPongoRef() || !PongoController || !PongoController->GetPawn())
	{
		if(PongoIndicator)
		{
			PopWidgetFromLayer(
				FGameplayTag::RequestGameplayTag(FName("UI.Layer.Game")),
				PongoIndicator
			);
			PongoIndicator = nullptr;
		}
		return;
	}
		
	

	if (GameInstanceRef->GetDeactivateHud()) {
		if(PongoIndicator)
		{
			PopWidgetFromLayer(
				FGameplayTag::RequestGameplayTag(FName("UI.Layer.Game")),
				PongoIndicator
			);
			PongoIndicator = nullptr;
		}

		if(DrumManager)
		{
			PopWidgetFromLayer(
				FGameplayTag::RequestGameplayTag(FName("UI.Layer.Overlay")),
				DrumManager
			);
			DrumManager = nullptr;
		}
		return;
	}
		
	
	if (bIsFirstRoom)
	{
		TargetLocation = GameModeRef->GetPongoRef()->GetActorLocation();
		float CurrentDistance = FVector::Dist(PongoStartLocation, TargetLocation);
		
		if (FMath::IsNearlyZero(CurrentDistance)||CurrentDistance < Tolerance)
		{
			if (PongoIndicator)
			{
			PopWidgetFromLayer(
				FGameplayTag::RequestGameplayTag(FName("UI.Layer.Game")),
				PongoIndicator
			);
			PongoIndicator = nullptr;
			}
			return;
		}
	}
	
	AActor* Target = GameModeRef->GetPongoRef();

	if (IsActorInView(PC, Target)) {
		if (PongoIndicator)
		{
			PopWidgetFromLayer(
				FGameplayTag::RequestGameplayTag(FName("UI.Layer.Game")),
				PongoIndicator
			);
			PongoIndicator = nullptr;
		}
	}


	
	if (!IsActorInView(PC, Target))
	{
		if (PC->GetPawn())
		{
			if (!PongoIndicator)
			{
				PongoIndicator = Cast<UTPP_PongoIndicator>(
					PushWidgetToLayer(
						FGameplayTag::RequestGameplayTag(FName("UI.Layer.Game")),
						PongoIndicatorClass
					)
				);
			}
			
			if (PongoIndicator)
			{
				PongoIndicator->SetImageSide(GetActorDirection(PC, Target));
				PongoIndicator->SetAngle(GetAngleToTargetCamera(PC, Target));
				PongoIndicator->EvaluateDistance(GetActorsDistance(PC->GetPawn(), Target));
				FIndicatorStatusData* FoundData = PongoStatusData.Find(PongoController->GetPongoState());
				if (FoundData && SavedPongoState != PongoController->GetPongoState())
				{
					SavedPongoState = PongoController->GetPongoState();
					FoundData->CachedPongoState = SavedPongoState;
					CurrentIndicatorStatusData  = *FoundData;
					PongoIndicator->SetIndicatorColorAndFrame(CurrentIndicatorStatusData);
				}
				
				FVector PongoVelocity = PongoController->GetPawn()->GetVelocity();
				if (!PongoVelocity.IsZero())
				{
					PongoIndicator->PlayPongoIconAnimation(true,DeltaTime);
				}
				else
				{
					PongoIndicator->PlayPongoIconAnimation(false,DeltaTime);
				}
			}
		}
	}
	else
	{
		if (PongoIndicator)
		{
			PopWidgetFromLayer(
				FGameplayTag::RequestGameplayTag(FName("UI.Layer.Game")),
				PongoIndicator
			);
			PongoIndicator = nullptr;
		}
	}
	
}

bool ATPP_HUDGame::IsActorInView(APlayerController* PC, AActor* Actor) const
{
	if (!PC || !Actor) return false;
	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer || !LocalPlayer->ViewportClient) return false;
	
	//Camera Data
	FSceneViewProjectionData ProjectionData;
	
	if (!LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport, ProjectionData))
		return false;

	// Calcolate view plus projection
	const FMatrix ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();

	// Volume Visibile from camera 
	FConvexVolume ViewFrustum;

	// Build Frustum
	GetViewFrustumBounds(ViewFrustum, ViewProjectionMatrix, false);
	
	const FBox Bounds = Actor->GetComponentsBoundingBox();
	
	return ViewFrustum.IntersectBox(Bounds.GetCenter(), Bounds.GetExtent());
}

FVector2D ATPP_HUDGame::GetActorDirection(APlayerController* PC, AActor* Actor) const
{
	FVector CamLocation = PC->PlayerCameraManager->GetCameraLocation();
	FRotator CamRotation = PC->PlayerCameraManager->GetCameraRotation();
	
	FVector Right = FRotationMatrix(CamRotation).GetUnitAxis(EAxis::Y);
	FVector Up    = FRotationMatrix(CamRotation).GetUnitAxis(EAxis::Z);

	FVector ToActor = (Actor->GetActorLocation() - CamLocation).GetSafeNormal();

	float RightValue = FVector::DotProduct(Right, ToActor);

	float UpValue    = FVector::DotProduct(Up, ToActor); 
	
	return FVector2D(RightValue, -UpValue);
}

float ATPP_HUDGame::GetActorsDistance(AActor* Actor1,AActor* Actor2) const
{
	float Distance = FVector::Distance(Actor1->GetActorLocation(), Actor2->GetActorLocation());
	return Distance;
}


float ATPP_HUDGame::GetAngleToTargetCamera(APlayerController* PC, AActor* Target) const
{
	if (!PC || !Target || !PC->PlayerCameraManager) return 0.f;

	FVector CamLocation = PC->PlayerCameraManager->GetCameraLocation();
	FRotator CamRotation = PC->PlayerCameraManager->GetCameraRotation();

	FVector ToTarget = (Target->GetActorLocation() - CamLocation).GetSafeNormal();
	
	FVector CamRight = FRotationMatrix(CamRotation).GetUnitAxis(EAxis::Y);
	FVector CamUp = FRotationMatrix(CamRotation).GetUnitAxis(EAxis::Z);
	
	float DotRight = FVector::DotProduct(ToTarget, CamRight);
	float DotUp = FVector::DotProduct(ToTarget, CamUp);
	float AngleRad = FMath::Atan2(-DotUp, DotRight); 
    
	return FMath::RadiansToDegrees(AngleRad);
}

void ATPP_HUDGame::ReadPongoStatus(ETPP_PongoState PongoState)
{
	const UEnum* EnumPtr = StaticEnum<ETPP_PongoState>();
	FString StateName = EnumPtr ? EnumPtr->GetNameStringByValue((int64)PongoState) : FString::FromInt((int32)PongoState);

	PONGO_LOG(Warning, "StateRecived: %s", *StateName);
	PONGO_SCREEN(FColor::Cyan, "NewState Pongo: %s", *StateName);
	

	//SavedPongoState = PongoState;
	if (PongoIndicator)
	{
		FIndicatorStatusData* FoundData = PongoStatusData.Find(PongoState);

		if (FoundData)
		{
			CurrentIndicatorStatusData = *FoundData;
			PONGO_LOG(Log, "DataFound State %s", *StateName);
			PONGO_SCREEN(FColor::Green, "Data OK %s", *StateName);
			CurrentIndicatorStatusData  = *FoundData;
			CurrentIndicatorStatusData.CachedPongoState = SavedPongoState;
			SavedPongoState = PongoState;
			PongoIndicator->SetIndicatorColorAndFrame(CurrentIndicatorStatusData);
			//PongoIndicator->PlayFeedbackAnimation(SavedPongoState);
		}
		else
		{
			PONGO_LOG(Error, "Missing Data in TMap %s!", *StateName);
			PONGO_SCREEN(FColor::Red, "ERROR: State %s not Config in Editor!", *StateName);
		}
	}

	const UTPP_GameInstance* GI = Cast<UTPP_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	bIsFirstRoom = (GameModeRef && GI->GetRoomIndexToLoad() <= 0);
}

FTutorialData* ATPP_HUDGame::GetDataTutorial(ECharacterTutorial Tutorial)
{
	if(GameInstanceRef && GameInstanceRef->GetDeactivateTutorial())
	{
		return MapTutorialData.Find(ECharacterTutorial::None);
	} 

	return MapTutorialData.Find(Tutorial);
}


void ATPP_HUDGame::ShowDrum()
{
	OnDrumActivated();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
		TimerHandle_ShowDrum,
		this,
		&ATPP_HUDGame::OnDrumDeactivated,
		TimerShowDrum,
		false
			);
	}
}

void ATPP_HUDGame::DisableInputPlayer()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	DisableInput(PC);
}

void ATPP_HUDGame::EnableInputPlayer()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	EnableInput(PC);

	if (PC)
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}

	FSlateApplication::Get().SetAllUserFocusToGameViewport();
}

void ATPP_HUDGame::RemoveDrumWidget()
{
	PopWidgetFromLayer(FGameplayTag::RequestGameplayTag(FName("UI.Layer.Overlay")),DrumManager);
	DrumManager = nullptr;
}

void ATPP_HUDGame::OnLoadingScreenActivated()
{
	if (!bRemoveLoadingScreen)
	{
		if (LoadingScreenClass)
		{
			LoadingScreenPtr = Cast<UTPP_LoadingScreen>(PushWidgetToLayer(
				FGameplayTag::RequestGameplayTag(FName("UI.Layer.Menu")),
				LoadingScreenClass));
			LoadingScreenPtr->InitializeLoadingScreen();
			DisableInputPlayer();
		}
	}
}

void ATPP_HUDGame::OnPetFinishedHUD()
{
	bHasPongoBeingPetOnce = true;

	if (GameModeRef && GameModeRef->GetPongoRef())
	{
		GameModeRef->GetPongoRef()->OnPetFinished.RemoveDynamic(this, &ATPP_HUDGame::OnPetFinishedHUD);
	}
}

void ATPP_HUDGame::DeactivateDrumCutscene()
{
	if (DrumManager)
	{
		PopWidgetFromLayer(
			FGameplayTag::RequestGameplayTag(FName("UI.Layer.Overlay")),
			DrumManager
		);

		DrumManager = nullptr;
	}
}

void ATPP_HUDGame::OnLoadingScreenDeactivated()
{
	if (LoadingScreenPtr)
	{
		LoadingScreenPtr->SetPlayerSpawned(true);	
	}
}

void ATPP_HUDGame::OnDrumActivated()
{
	if (DrumManager)
	{
		PopWidgetFromLayer(
			FGameplayTag::RequestGameplayTag(FName("UI.Layer.Overlay")),
			DrumManager
		);

		DrumManager = nullptr;
	}
	
	if (DrumManagerClass)
	{
		if (TimerHandle_RemoveDrum.IsValid())
		{
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().ClearTimer(TimerHandle_RemoveDrum);
			}
		}
		
		if (TimerHandle_ShowDrum.IsValid())
		{
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().ClearTimer(TimerHandle_ShowDrum);
			}
		}

		if (!DrumManager)
		DrumManager = Cast<UTPP_DrumManager>(PushWidgetToLayer(FGameplayTag::RequestGameplayTag(FName("UI.Layer.Overlay")),DrumManagerClass));
		if (DrumManager)
			DrumManager->InitializeDrum(); 
		else
			UE_LOG(LogTemp, Warning, TEXT("DrumNotSpawned"));
			
	}

}
void ATPP_HUDGame::OnDrumDeactivated()
{
	if (DrumManager)
	{
		DrumManager->PlayAnimationDrumOut();
	
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
			TimerHandle_RemoveDrum,
			this,
			&ATPP_HUDGame::RemoveDrumWidget,
			0.5,
			false
				);
		}
	  }
}

void ATPP_HUDGame::OnComeHereActivated()
{
	if (DrumManager)
	{
		DrumManager->ExecuteHudCommandDrum(EDrumUIAction::ComeHere,true);
	}
}
void ATPP_HUDGame::OnGoThereActivated()
{
	if (DrumManager)
	{
		DrumManager->ExecuteHudCommandDrum(EDrumUIAction::GoThere,DrumManager->GetDrum()->bGoThereActive);
	}
}
void ATPP_HUDGame::OnGoThereCompleted()
{
	
}
void ATPP_HUDGame::OnInteractActivated()
{
	if (DrumManager)
	{
		DrumManager->ExecuteHudCommandDrum(EDrumUIAction::Interact,DrumManager->GetDrum()->bInteractActive);
	}
}

void ATPP_HUDGame::OnStopActivated()
{
	if (DrumManager)
	{
		DrumManager->ExecuteHudCommandDrum(EDrumUIAction::Stop,true);
	}
}

void ATPP_HUDGame::OnPauseMenuOpen()
{
	if (DrumManager)
	{
		DrumManager->SaveDataClosedMenu();
		PopWidgetFromLayer(
			FGameplayTag::RequestGameplayTag(FName("UI.Layer.Overlay")),
			DrumManager
		);
		bWasDrumInUse = true;
		DrumManager = nullptr;
	}
	else
	{
		bWasDrumInUse = false;
	}

	if (PongoIndicator)
	{
		PopWidgetFromLayer(
			FGameplayTag::RequestGameplayTag(FName("UI.Layer.Game")),
			PongoIndicator
		);
		PongoIndicator = nullptr;
	}
}

void ATPP_HUDGame::OnPauseMenuClosed()
{
	if (bWasDrumInUse)
	{
		DrumManager = Cast<UTPP_DrumManager>(PushWidgetToLayer(FGameplayTag::RequestGameplayTag(FName("UI.Layer.Overlay")),DrumManagerClass));
		if (DrumManager)
		{
			DrumManager->LoadDataClosedMenu();
		}
		bWasDrumInUse = false;
	}
}

void ATPP_HUDGame::OnGameOverCalled(ECharacterDeath CharacterDeath,EDeathCause DeathCause)
{
	GameOverWidget = Cast<UTPP_GameOverWidget>(PushWidgetToLayer(FGameplayTag::RequestGameplayTag(FName("UI.Layer.Menu")),GameOverClass));
	
	if (DrumManager)
	{
		PopWidgetFromLayer(
		FGameplayTag::RequestGameplayTag(FName("UI.Layer.Overlay")),
		DrumManager
		);
		DrumManager = nullptr;
	}
	if (PongoIndicator)
	{
		PopWidgetFromLayer(
				FGameplayTag::RequestGameplayTag(FName("UI.Layer.Game")),
				PongoIndicator
			);
		PongoIndicator = nullptr;
	}
	
	if (GameOverWidget)
	{
		if (const FDeathText* FoundDeathText = DeathTextMap.Find(DeathCause))
		{
			GameOverWidget->InizializeGameOverWidget(
				CharacterDeath,
				FoundDeathText->DeathMessage
			);
		}
	}
}
