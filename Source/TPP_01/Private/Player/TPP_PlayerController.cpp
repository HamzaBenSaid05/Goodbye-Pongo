#include "Player/TPP_PlayerController.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/TPP_CharacterBase.h"
#include "Characters/Player/TPP_PlayerCharacter.h"
#include "Drum/TPP_Drum.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/GameInstances/TPP_GameInstance.h"
#include "Item/Interactable/TPP_InteractableItem.h"

void ATPP_PlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ATPP_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	LocalPlayer = Cast<ULocalPlayer>(Player);

	InputSubsystem = Cast<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>());
	if (InputSubsystem)
	{
		InputSubsystem->AddMappingContext(LocomotionContext.LoadSynchronous(), 0);
	}

	PlayerCharacter = Cast<ATPP_PlayerCharacter>(InPawn);
}

void ATPP_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATPP_PlayerController::Move);

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATPP_PlayerController::Jump);
		EnhancedInputComponent->BindAction
		(JumpAction,
			ETriggerEvent::Canceled,
			this,
			&ATPP_PlayerController::StopJumping);
		EnhancedInputComponent->BindAction
		(JumpAction,
			ETriggerEvent::Completed,
			this,
			&ATPP_PlayerController::StopJumping);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ATPP_PlayerController::Interact);
		// Crouching
		EnhancedInputComponent->BindAction
		(CrouchAction,
			ETriggerEvent::Triggered,
			this,
			&ATPP_PlayerController::Crouch);
		// UI
		EnhancedInputComponent->BindAction(OptionMenuAction, ETriggerEvent::Started, this, &ATPP_PlayerController::OpenOptionMenu);
	}
}

void ATPP_PlayerController::SetupDrumInput()
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		InputSubsystem->AddMappingContext(DrumContext.LoadSynchronous(), 2);
		// Drum
		EnhancedInputComponent->BindAction(DrumToggle, ETriggerEvent::Triggered, this, &ATPP_PlayerController::DrumActive);
		//EnhancedInputComponent->BindAction(DrumToggle, ETriggerEvent::Started, this, &ATPP_PlayerController::DrumActiveStarted);
		EnhancedInputComponent->BindAction(DrumToggle, ETriggerEvent::Completed, this, &ATPP_PlayerController::DrumInactive);

		//Drum Action
		EnhancedInputComponent->BindAction(ComeHereAction, ETriggerEvent::Completed, this, &ATPP_PlayerController::ComeHere);
		EnhancedInputComponent->BindAction(StopAction, ETriggerEvent::Completed, this, &ATPP_PlayerController::Stop);
		EnhancedInputComponent->BindAction(PongoInteractAction, ETriggerEvent::Completed, this, &ATPP_PlayerController::PongoInteract);
		EnhancedInputComponent->BindAction(GoThereAction, ETriggerEvent::Started, this, &ATPP_PlayerController::GoThereStarted);
		EnhancedInputComponent->BindAction(GoThereAction, ETriggerEvent::Triggered, this, &ATPP_PlayerController::GoThere);
		EnhancedInputComponent->BindAction(GoThereAction, ETriggerEvent::Completed, this, &ATPP_PlayerController::GoThereCompleted);
	}
}

void ATPP_PlayerController::ToggleLocomotionMappingContext(bool bEnable)
{
	LocalPlayer = Cast<ULocalPlayer>(Player);

	if (!LocalPlayer)
	{
		return;
	}

	if (!InputSubsystem)
	{
		return;
	}

	UInputMappingContext* MappingContext = LocomotionContext.LoadSynchronous();

	if (!MappingContext)
	{
		return;
	}

	const bool bAlreadyHasContext =
	InputSubsystem->HasMappingContext(MappingContext);

	if (bEnable)
	{
		if (!bAlreadyHasContext)
		{
			InputSubsystem->AddMappingContext(MappingContext, 0);
		}
	}
	else
	{
		if (bAlreadyHasContext)
		{
			InputSubsystem->RemoveMappingContext(MappingContext);
		}
	}
}

void ATPP_PlayerController::ToggleDrumMappingContext(bool bEnable)
{
	LocalPlayer = Cast<ULocalPlayer>(Player);

	if (!LocalPlayer)
	{
		return;
	}

	if (!InputSubsystem)
	{
		return;
	}

	UInputMappingContext* MappingContext = DrumContext.LoadSynchronous();

	if (!MappingContext)
	{
		return;
	}

	const bool bAlreadyHasContext =
	InputSubsystem->HasMappingContext(MappingContext);

	if (bEnable)
	{
		if (!bAlreadyHasContext)
		{
			InputSubsystem->AddMappingContext(MappingContext, 0);
		}
	}
	else
	{
		if (bAlreadyHasContext)
		{
			InputSubsystem->RemoveMappingContext(MappingContext);
		}
	}
}

void ATPP_PlayerController::Kill_Implementation(bool bSoftReset, const ECharacterDeath CharacterDeath, const EDeathCause Cause)
{
	// TODO: Handle softReset
	InputSubsystem->RemoveMappingContext(LocomotionContext.LoadSynchronous());
	InputSubsystem->RemoveMappingContext(DrumContext.LoadSynchronous());
}

void ATPP_PlayerController::HandleReset()
{
	InputSubsystem->AddMappingContext(LocomotionContext.LoadSynchronous(), 0);
	if (const UTPP_GameInstance* GI = Cast<UTPP_GameInstance>(GetGameInstance()))
	{
		if (const UTPP_SaveGame* SaveSystem = GI->GetSaveGameObject())
		{
			if (SaveSystem->bHasDrum)
			{
				InputSubsystem->AddMappingContext(DrumContext.LoadSynchronous(), 2);
			}
		}
	}
}

void ATPP_PlayerController::Move(const FInputActionValue& Value)
{
	if (!IsValid(PlayerCharacter))
		return;
	const FVector2D InputValue = Value.Get<FVector2D>();
	if (PlayerCharacter->GetIsDragging())
	{
		if (PlayerCharacter->bHasBlockedBackMovement)
		{
			FVector Forward = GetCharacter()->GetActorForwardVector();
			Forward.Z = 0;
			Forward.Normalize();

			FVector InputVector = FVector(FMath::RoundHalfToZero(InputValue.X), FMath::RoundHalfToZero(InputValue.Y), 0);
			InputVector.Normalize();

			float ForwardAmount = FVector::DotProduct(InputVector, Forward);
			if (ForwardAmount < 0.f)
				return;

			GetCharacter()->AddMovementInput(Forward, ForwardAmount);
		}
		else if (PlayerCharacter->bHasBlockedForMovement)
		{
			FVector Forward = GetCharacter()->GetActorForwardVector();
			Forward.Z = 0;
			Forward.Normalize();

			FVector InputVector = FVector(FMath::RoundHalfToZero(InputValue.X), FMath::RoundHalfToZero(InputValue.Y), 0);
			InputVector.Normalize();

			float ForwardAmount = FVector::DotProduct(InputVector, Forward);
			if (ForwardAmount > 0.f)
				return;

			GetCharacter()->AddMovementInput(Forward, ForwardAmount);
		}
		else
		{
			FVector Forward = GetCharacter()->GetActorForwardVector();
			Forward.Z = 0;
			Forward.Normalize();

			FVector InputVector = FVector(FMath::RoundHalfToZero(InputValue.X), FMath::RoundHalfToZero(InputValue.Y), 0);
			InputVector.Normalize();

			float ForwardAmount = FVector::DotProduct(InputVector, Forward);
			GetCharacter()->AddMovementInput(Forward, ForwardAmount);
		}
	}
	else
	{
		GetCharacter()->AddMovementInput(FVector::ForwardVector, InputValue.X);
		GetCharacter()->AddMovementInput(FVector::RightVector, InputValue.Y);
	}
}

void ATPP_PlayerController::Jump()
{
	bool Jump = PlayerCharacter->CanJump();
	if (Jump == true && PlayerCharacter->GetDrum() != nullptr)
	{
		DrumInactive();
	}
}

void ATPP_PlayerController::StopJumping() { GetCharacter()->StopJumping(); }

void ATPP_PlayerController::Crouch()
{
	if (PlayerCharacter->GetIsDragging())
		return;

	if (!GetCharacter()->GetCharacterMovement()->bWantsToCrouch)
	{
		if (GetCharacter()->GetCharacterMovement()->IsWalking())
			GetCharacter()->GetCharacterMovement()->bWantsToCrouch = true;
		if (PlayerCharacter->GetDrum() != nullptr) { DrumInactive(); }
	}
	else
		GetCharacter()->GetCharacterMovement()->bWantsToCrouch = false;
}

void ATPP_PlayerController::Uncrouch()
{
	GetCharacter()->GetCharacterMovement()->bWantsToCrouch = false;
	PlayerCharacter->SetActiveDrum(true);
}

void ATPP_PlayerController::Interact()
{
	if (PlayerCharacter->GetIsActiveDrum())
		return;

	if (PlayerCharacter->GetInteractableItem()) { PlayerCharacter->Interact(PlayerCharacter->GetInteractableItem()->InteractWith()); }
	else if (PlayerCharacter->GetDetectedItem()) { PlayerCharacter->Interact(PlayerCharacter->GetDetectedItem()->InteractWith()); }
	else { PlayerCharacter->PetPongo(); }

	// Call InteractLinkHUD
	OnInteractActivated.Broadcast();
}

void ATPP_PlayerController::DrumActive()
{
	if (!CheckDrumActive())
		return;
	PlayerCharacter->ComeHereDrumAction();

	DrumActiveStarted();
	//InputSubsystem->AddMappingContext(DrumActionContext.LoadSynchronous(), 1);
}

void ATPP_PlayerController::DrumActiveStarted()
{
	if (bDrumStarted || !PlayerCharacter->GetIsLanded())
		return;
	if (!CheckDrumActive())
		return;

	PlayerCharacter->PlayDrumStateSound(true);
	bDrumStarted = true;
	InputSubsystem->AddMappingContext(DrumActionContext.LoadSynchronous(), 1);
}

void ATPP_PlayerController::DrumInactive()
{
	bDrumStarted = false;

	if (PlayerCharacter->GetDrum().Get()->bGoThereInUse)
	{
		PlayerCharacter->GoThereActionCancelled();
	}
	else
	{
		if(PlayerCharacter->GetIsActiveDrum())
			PlayerCharacter->PlayDrumStateSound(false);
		PlayerCharacter->SetActiveDrum(false);
	}
	
	InputSubsystem->RemoveMappingContext(DrumActionContext.LoadSynchronous());
}

bool ATPP_PlayerController::CheckDrumActive() const
{
	bool bIsValid = true;
	if (!PlayerCharacter->GetCharacterMovement()->IsMovingOnGround()) { bIsValid = false; }
	if (PlayerCharacter->GetCharacterMovement()->bWantsToCrouch)
	{
		PlayerCharacter->SetActiveDrum(false);
		bIsValid = false;
	}
	if (PlayerCharacter->GetInteractableItem() != nullptr || PlayerCharacter->GetIsDragging())
		bIsValid = false;
	PlayerCharacter->SetActiveDrum(bIsValid);
	return bIsValid;
}

void ATPP_PlayerController::GoThereStarted()
{
	PlayerCharacter->GoThereActionStarted();
	// lINK IN HUD
	OnGoThereActivated.Broadcast();
}

void ATPP_PlayerController::GoThere()
{
	PlayerCharacter->GoThereAction();
}

void ATPP_PlayerController::GoThereCompleted()
{
	PlayerCharacter->GoThereActionCompleted();

	//lINK IN Hud
	OnGoThereCompleted.Broadcast();
}

void ATPP_PlayerController::PongoInteract()
{
	PlayerCharacter->PongoInteractAction();
	OnInteractActivated.Broadcast();
}

void ATPP_PlayerController::ComeHere()
{
	PlayerCharacter->ComeHereDrumCompleted();
	// Link in Hud
	OnComeHereActivated.Broadcast();
}

void ATPP_PlayerController::Stop()
{
	PlayerCharacter->StopDrumAction();
	// Link in Hud
	OnStopActivated.Broadcast();
}

void ATPP_PlayerController::OpenOptionMenu()
{
	OnPauseMenuOpen.Broadcast();
	HandleOptionMenu();
}
