#include "Characters/Pongo/TPP_Pongo.h"

#include "AI/AIController/TPP_PongoController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/Enum/TPP_PongoEnum.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Characters/Player/TPP_PlayerCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "DataAsset/VFX/TPP_FurTransitionSocketMap.h"
#include "Gameplay/GameModes/TPP_GameMode.h"
#include "Item/Interactable/TPP_InteractableItem.h"
#include "Kismet/KismetMathLibrary.h"
#include "DataAsset/InteractActions/TPP_DragAction_DA.h"

ATPP_Pongo::ATPP_Pongo()
{
	FurMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FurMesh"));
	FurMesh->SetupAttachment(GetMesh());

	AIControllerClass = ATPP_PongoController::StaticClass();
	PetBarComponent = CreateDefaultSubobject<UTPP_PetBar>("PetBarComponent");

	ReactionsVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Reactions VFX"));
	ReactionsVFX->SetupAttachment(GetRootComponent());
}

void ATPP_Pongo::EndDragAction() { Super::EndDragAction(); }

void ATPP_Pongo::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = 0;
	SoundComponent->PlaySound(TagJumpLand, GetActorLocation(), AudioCompLand, Parameter);
	if (UCharacterMovementComponent* CMC = GetCharacterMovement())
		CMC->bOrientRotationToMovement = true;
	if (PendingJumpItem.Get())
	{
		PendingJumpItem->GetBoxNavMesh()->bDynamicObstacle = true;
		PendingJumpItem = nullptr;
	}
	if (ATPP_PongoController* PC = Cast<ATPP_PongoController>(GetController()))
	{
		TWeakObjectPtr<ATPP_PongoController> WeakPC = PC;
		TogglePlayerCollision_Implementation(true);
		GetWorldTimerManager().SetTimerForNextTick
		([WeakPC]()
		{
			if (WeakPC.IsValid())
			{
				WeakPC->FlushPendingEvents();
#if WITH_EDITOR
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, "Landed: Flushing Pending Events");
				}
#endif
			}
		});
	}
}

void ATPP_Pongo::SetSpeedBasedOnState(uint8 State)
{
	SetCharacterSpeedBasedOnState(static_cast<ETPP_PongoState>(State));
}

void ATPP_Pongo::ResetColor()
{
	SetCharacterColorBasedOnState(ETPP_PongoState::Idle);
}

void ATPP_Pongo::SetColor_Implementation(FLinearColor NewColor)
{
	StartChangingColor(NewColor);
}

void ATPP_Pongo::TogglePlayerCollision_Implementation(bool bEnable)
{
	if (bEnable)
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
	else
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
}

void ATPP_Pongo::Jump(const FVector& EndPos, const float& DestinationZOffset)
{
	if (IsInteracting() && GetInteractableItem() && GetInteractableItem()->GetInteractData())
	{
		
		if (GetInteractableItem()->GetInteractData()->GetObjectInteractionType() == EObjectInteractionType::Medium)
		{
			
			FRotator DropRotation = GetDropRotation();
			float SnappedYaw = FMath::GridSnap(FRotator::NormalizeAxis(DropRotation.Yaw), 90.0f);
			FRotator CleanRotation = FRotator(0.0f, SnappedYaw, 0.0f);

			SetActorRotation(CleanRotation);
			if (Controller)
			{
				Controller->SetControlRotation(CleanRotation);
			}
		}
		PendingJumpItem = GetInteractableItem();
		EndInteract(GetInteractableItem()->GetInteractData());
	}
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = 1;
	SoundComponent->PlaySound(TagJumpLand, GetActorLocation(), AudioCompJump, Parameter);
	// Start Position
	const FVector& StartPos = GetActorLocation();

	// Velocity use to launch the character
	FVector OutLaunchVelocity = FVector::ZeroVector;

	// Adding Z Offset to destination
	const FVector EndPosZOffset = FVector(EndPos.X, EndPos.Y, EndPos.Z + DestinationZOffset);

	// Calculate rotation to face the destination, only if there's meaningful horizontal movement
	//FVector LaunchDirXY = FVector(OutLaunchVelocity.X, OutLaunchVelocity.Y, 0.f);
	//const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(StartPos, EndPosZOffset);
	//FRotator NewRot(0.f, LookAt.Yaw, 0.f);
	//SetActorRotation(NewRot);
	//if (Controller) { Controller->SetControlRotation(NewRot); }

	bool bHaveSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc
	(
		this,
		OutLaunchVelocity,
		StartPos,
		EndPosZOffset,
		0,
		JumpArc
	);

	// If a trajectory is found, launch the character
	if (bHaveSolution)
	{
		FVector LaunchDirXY = FVector(OutLaunchVelocity.X, OutLaunchVelocity.Y, 0.f);
		if (LaunchDirXY.SizeSquared() > 100.f)
		{
			FRotator NewRot(0.f, LaunchDirXY.Rotation().Yaw, 0.f);
			SetActorRotation(NewRot);
			if (Controller) Controller->SetControlRotation(NewRot);
		}

		if (UCharacterMovementComponent* CMC = GetCharacterMovement())
		{
			CMC->bOrientRotationToMovement = false;
		}
		TogglePlayerCollision_Implementation(false);
		LaunchCharacter(OutLaunchVelocity, true, true);
	}
}

void ATPP_Pongo::GetBoxTraceIgnoredActors(TArray<AActor*>& OutIgnored)
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APawn* PlayerPawn = PC->GetPawn())
			OutIgnored.Add(PlayerPawn);
	}
}

void ATPP_Pongo::AddGameplayTag_Implementation(FGameplayTag Tag)
{
	OwnedTags.AddTag(Tag);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage
		(
			-1,
			5.f,
			FColor::Green,
			FString::Printf
			(TEXT("Pongo Tag added: %s | All Tags: %s"),
				*Tag.ToString(),
				*OwnedTags.ToStringSimple())
		);
	}
}

void ATPP_Pongo::SetCharacterSpeed(const float NewSpeed) const { GetCharacterMovement()->MaxWalkSpeed = NewSpeed; }

void ATPP_Pongo::SetCharacterSpeedBasedOnState(ETPP_PongoState State)
{
	UE_LOG(LogTemp, Log, TEXT("State: %i"), State);
	bool bIsMoving = ControllerRef->GetIsMoving();
	//bool  bIsValidState = State == ETPP_PongoState::Idle || State == ETPP_PongoState::Stroll;
	bool bShouldbBlock = !bIsMoving; //&& ! bIsValidState;
	TogglePlayerCollision_Implementation(bShouldbBlock);
	switch (State)
	{
	case ETPP_PongoState::Idle:
		if (State != ETPP_PongoState::Idle)
		{
			SpeedMultiplier = 1.f;
		}
		BaseMovementSpeed = DefaultMovementSpeed * SpeedMultiplier;

#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage
			(4,
				5.f,
				FColor::White,
				FString::Printf
				(
					TEXT("Speed Multiplier SetCharacterSpeedBasedOnState: %f"),
					SpeedMultiplier));
		}
#endif
		SetCharacterSpeed(BaseMovementSpeed);
		break;

	case ETPP_PongoState::Fear:
		SetCharacterSpeed(ScaredMovementSpeed);
		break;

	case ETPP_PongoState::FearPlayer:
		SetCharacterSpeed(ScaredFromPlayerMovementSpeed);
		break;

	case ETPP_PongoState::Curiosity:
		SetCharacterSpeed(CuriousMovementSpeed);
		break;

	case ETPP_PongoState::Rage:
		SetCharacterSpeed(RagingMovementSpeed);
		break;

	case ETPP_PongoState::Trust:
		SetCharacterSpeed(TrustingMovementSpeed);
		break;

	case ETPP_PongoState::Scripted:
		SetCharacterSpeed(ScriptedMovementSpeed);
		break;

	default:
		SetCharacterSpeed(BaseMovementSpeed);
		break;
	}
}

void ATPP_Pongo::SetCharacterColorBasedOnState(ETPP_PongoState State)
{
	bool bIsMoving = ControllerRef->GetIsMoving();
	//bool  bIsValidState = State == ETPP_PongoState::Idle || State == ETPP_PongoState::Stroll;
	bool bShouldbBlock = !bIsMoving; //&& ! bIsValidState;
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	TogglePlayerCollision_Implementation(bShouldbBlock);
	switch (State)
	{
	case ETPP_PongoState::Idle:
		SocketToUse = FurTransitionDA->SocketMap.FindRef(ETPP_PongoState::Idle);
		StartChangingColor(BaseMovementColor, true);
		break;
	case ETPP_PongoState::Fear:
		SocketToUse = FurTransitionDA->SocketMap.FindRef(ETPP_PongoState::Fear);
		StartChangingColor(ScaredMovementColor);
		PlayReactionVFX(ETPP_PongoReactions::Fear);
		Parameter.Int = 0;
		SoundComponent->PlaySound(TagEmotional, GetActorLocation(), AudioCompState, Parameter);
		break;

	case ETPP_PongoState::FearPlayer:
		SocketToUse = FurTransitionDA->SocketMap.FindRef(ETPP_PongoState::FearPlayer);
		StartChangingColor(ScaredFromPlayerMovementColor);
		PlayReactionVFX(ETPP_PongoReactions::Fear);
		Parameter.Int = 0;
		SoundComponent->PlaySound(TagEmotional, GetActorLocation(), AudioCompState, Parameter);
		break;

	case ETPP_PongoState::Curiosity:
		SocketToUse = FurTransitionDA->SocketMap.FindRef(ETPP_PongoState::Curiosity);
		StartChangingColor(CuriousMovementColor);
		PlayReactionVFX(ETPP_PongoReactions::Curiosity);
		Parameter.Int = 1;
		SoundComponent->PlaySound(TagEmotional, GetActorLocation(), AudioCompState, Parameter);
		GetWorld()->GetTimerManager().SetTimer
		(
			CuriosityTimerHandle,
			this,
			&ATPP_Pongo::CheckCuriosityCooldown,
			CuriosityCooldown,
			true
		);
		break;

	case ETPP_PongoState::Rage:
		SocketToUse = FurTransitionDA->SocketMap.FindRef(ETPP_PongoState::Rage);
		StartChangingColor(RagingMovementColor);
		Parameter.Int = 2;
		SoundComponent->PlaySound(TagEmotional, GetActorLocation(), AudioCompState, Parameter);
		break;

	case ETPP_PongoState::Trust:
		StartChangingColor(TrustingMovementColor);
		Parameter.Int = 3;
		SoundComponent->PlaySound(TagEmotional, GetActorLocation(), AudioCompState, Parameter);
		break;

	case ETPP_PongoState::Stroll:
		PlayReactionVFX(ETPP_PongoReactions::Strolling);
		break;
	}
}

void ATPP_Pongo::StartChangingColor(FLinearColor NewColor, bool bIsIdle, bool bIsBonding)
{
	if (bIsBonding)
	{
		BaseMovementColor = NewColor;

		SocketToUse = FurTransitionDA->SocketMap.FindRef(ETPP_PongoState::Trust);
		
		if (!bIsIdle)
			return;
	}

	if (SocketToUse.IsEqual(""))
	{
		SocketToUse = FurTransitionDA->SocketMap.FindRef(ETPP_PongoState::Fear);
	}
	
	StartColor = CurrentColor;
	TargetColor = NewColor;
	ElapsedTime = 0.f;

	Fur_MaterialInst->SetVectorParameterValue(TEXT("NewColor"), TargetColor);

	bIsLerpingColor = true;
}

void ATPP_Pongo::CheckCuriosityCooldown()
{
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = 1;
	if (AudioCompState)
		AudioCompState->Stop();
	SoundComponent->PlaySound(TagEmotional, GetActorLocation(), AudioCompState, Parameter);
}

void ATPP_Pongo::BeginPlay()
{
	Super::BeginPlay();
	if (ATPP_GameMode* GameMode = Cast<ATPP_GameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (ATPP_PlayerCharacter* PlayerCharacter = GameMode->GetPlayerCharacterRef())
		{
			GetMesh()->IgnoreActorWhenMoving(PlayerCharacter, true);
			GetCapsuleComponent()->IgnoreActorWhenMoving(PlayerCharacter, true);
			if (UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent())
			{
				GetMesh()->IgnoreComponentWhenMoving(Capsule, true);
				GetCapsuleComponent()->IgnoreComponentWhenMoving(Capsule, true);
			}
		}
	}
}

void ATPP_Pongo::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (bIsPlayingReactionVFX)
	{
		ReactionsVFX->SetWorldLocation(GetActorLocation() + FVector(150,0,120));
	}
	
	if (!bIsLerpingColor) { return; }

	ElapsedTime += DeltaSeconds;
	
	const FVector CollarPos = FurMesh->GetSocketLocation(SocketToUse);
	Fur_MaterialInst->SetVectorParameterValue("CollarPos", CollarPos);

	const float Alpha = FMath::Clamp(ElapsedTime / LerpDuration, 0.f, 1.f);
	Fur_MaterialInst->SetScalarParameterValue("Radius", Alpha);

	if (Alpha >= 1.f)
	{
		Fur_MaterialInst->SetVectorParameterValue(TEXT("NewColor"), TargetColor);
		Fur_MaterialInst->SetVectorParameterValue(TEXT("OldColor"), TargetColor);
		Fur_MaterialInst->SetScalarParameterValue("Radius", 0);

		SocketToUse = "";
		bIsLerpingColor = false;
	}

	// CurrentColor = FMath::Lerp(StartColor, TargetColor, Alpha);
	// for (UMaterialInstanceDynamic* DynMat : DynamicMaterials)
	// {
	// 	if (DynMat) { DynMat->SetVectorParameterValue(TEXT("BaseColor"), CurrentColor); }
	// }
	//
	// if (Alpha >= 1.f)
	// {
	// 	CurrentColor = TargetColor;
	// 	for (UMaterialInstanceDynamic* DynMat : DynamicMaterials)
	// 	{
	// 		if (DynMat) { DynMat->SetVectorParameterValue(TEXT("BaseColor"), TargetColor); }
	// 	}
	// 	bIsLerpingColor = false;
	// }
}

void ATPP_Pongo::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (!NewController)
		return;
	if (Cast<ATPP_PongoController>(NewController))
	{
		ControllerRef = Cast<ATPP_PongoController>(NewController);
		ControllerRef->OnPongoChangedState.AddDynamic(this, &ATPP_Pongo::SetCharacterColorBasedOnState);
		ControllerRef->OnPongoChangedState.AddDynamic(this, &ATPP_Pongo::SetCharacterSpeedBasedOnState);
	}

	// if (GetMesh())
	// {
	// 	const int32 NumMaterials = GetMesh()->GetNumMaterials();
	//
	// 	for (int32 i = 0; i < NumMaterials; i++)
	// 	{
	// 		if (i != 2)
	// 			continue;
	// 		if (UMaterialInstanceDynamic* DynMat = GetMesh()->CreateAndSetMaterialInstanceDynamic(i))
	// 		{
	// 			DynamicMaterials.Add(DynMat);
	// 		}
	// 	}
	// }

	Fur_MaterialInst = FurMesh->CreateDynamicMaterialInstance(0);
	if (Fur_MaterialInst)
	{
		Fur_MaterialInst->SetVectorParameterValue(TEXT("OldColor"), BaseMovementColor);
		Fur_MaterialInst->SetVectorParameterValue(TEXT("NewColor"), BaseMovementColor);
	}

	StartColor = BaseMovementColor;
	CurrentColor = BaseMovementColor;
	TargetColor = BaseMovementColor;

	// for (UMaterialInstanceDynamic* DynMat : DynamicMaterials)
	// {
	// 	if (DynMat) { DynMat->SetVectorParameterValue(TEXT("BaseColor"), BaseMovementColor); }
	// }
	StartChangingColor(BaseMovementColor);
	SetCharacterSpeed(BaseMovementSpeed);
	DefaultMovementSpeed = BaseMovementSpeed;
}

void ATPP_Pongo::OnItemDetached(ATPP_InteractableItem* Item)
{
	if (PendingJumpItem.Get())
		return;

	if (Item)
		Item->GetBoxNavMesh()->bDynamicObstacle = true;
}
