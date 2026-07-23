#include "Characters/Player/TPP_PlayerCharacter.h"

#include "NiagaraComponent.h"
#include "Characters/Pongo/TPP_Pongo.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/WidgetComponent.h"
#include "CutScene/TPP_LevelSequencePlayer.h"
#include "Drum/TPP_Drum.h"
#include "Drum/TPP_GoThereMarker.h"
#include "EventProxies/TPP_AICommsProxySubsystem.h"
#include "EventProxies/AICommsEvents/Petting/TPP_AIComms_OnPetEvent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Player/TPP_PlayerController.h"
#include "Item/Interactable/TPP_OrbItem.h"
#include "DataAsset/VFX/Drum/TPP_DrumCommandsColors.h"
#include "Gameplay/GameInstances/TPP_GameInstance.h"
#include "DataAsset/InteractActions/TPP_InteractAction_DA.h"
#include "SaveSystem/TPP_SaveGame.h"

ATPP_PlayerCharacter::ATPP_PlayerCharacter()
{
	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	WidgetComp->SetupAttachment(RootComponent);
	WidgetComp->SetVisibility(false);
	WidgetComp->SetWidgetSpace(EWidgetSpace::World);
	WidgetComp->SetUsingAbsoluteRotation(true);
	WidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DrumPivot = CreateDefaultSubobject<USceneComponent>(TEXT("DrumPivot"));
	DrumPivot->SetupAttachment(GetMesh());
	DrumOrbitObject = CreateDefaultSubobject<USceneComponent>(TEXT("DrumOrbitObject"));
	DrumOrbitObject->SetupAttachment(DrumPivot);
	DrumOrbitObject->SetRelativeLocation(FVector(0.f, 50.f, 0.f));

	BraceletFadeoutTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Bracelet FadeOut Timeline"));
}

void ATPP_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SavedStartRadius = GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	SavedStartHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SavedDistancePlayerDrum = FMath::Abs(DrumOrbitObject->GetRelativeLocation().Y);

	if (OrbitCurve)
	{
		FOnTimelineFloat Progress;
		Progress.BindUFunction(this, FName("HandleOrbitProgress"));

		OrbitTimeline.AddInterpFloat(OrbitCurve, Progress);

		FOnTimelineEvent Finished;
		Finished.BindUFunction(this, FName("OnOrbitFinished"));

		OrbitTimeline.SetTimelineFinishedFunc(Finished);

		FOnTimelineEvent Start, End;
		Start.BindUFunction(this, FName("OnOrbitStart"));
		End.BindUFunction(this, FName("OnOrbitEnd"));
		OrbitTimeline.AddEvent(0 + OrbitTimeline.GetTimelineLength() * 0.05, MoveTemp(Start));
		OrbitTimeline.AddEvent(OrbitTimeline.GetTimelineLength() - OrbitTimeline.GetTimelineLength() * 0.05,
		                       MoveTemp(End));

		OrbitTimeline.SetLooping(false);
	}

	BraceletMaterialInstance = GetMesh()->CreateDynamicMaterialInstance(BraceletMaterialSlotIndex);
	BraceletMaterialInstance->GetVectorParameterValue(FName("Color"), StartingBraceletColor);
	if (BraceletFadeOutCurve)
	{
		FOnTimelineFloat BraceletFadeProgress;
		BraceletFadeProgress.BindDynamic(this, &ATPP_PlayerCharacter::HandleBraceletFadeOutProgress);

		FOnTimelineEvent BraceletFadeFinished;
		BraceletFadeFinished.BindDynamic(this, &ATPP_PlayerCharacter::OnBraceletFadeOutFinished);

		BraceletFadeoutTimeline->AddInterpFloat(BraceletFadeOutCurve.Get(), MoveTemp(BraceletFadeProgress));
		BraceletFadeoutTimeline->SetTimelineFinishedFunc(MoveTemp(BraceletFadeFinished));
	}
}

void ATPP_PlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	OrbitTimeline.TickTimeline(DeltaSeconds);

	if (bIsApproachingDragTarget)
		TickDragApproach(DeltaSeconds);

	FHitResult HitPet = LineTrace(GetActorLocation(),
	                              GetActorLocation() + (GetActorForwardVector() * PetInteractionLenght));

	if (HitPet.GetActor() != nullptr)
	{
		if (HitPet.GetActor()->ActorHasTag("Pongo"))
		{
			APawn* Pawn = Cast<APawn>(HitPet.GetActor());
			ITPP_AIPettable* PettableInterface = Cast<ITPP_AIPettable>(Pawn->GetController());
			if (PettableInterface && PettableInterface->CanBePet() && !bIsPetting)
			{
				ActivateWidget(HitPet.GetActor()->GetActorLocation());
			}
			else
			{
				if (WidgetComp->IsVisible())
					DeactivateWidget();
			}
		}
	}
	else { DeactivateWidget(); }
}

void ATPP_PlayerCharacter::VisualizeUIItemInteraction()
{
	ATPP_InteractableItem* Item = GetDetectedItem();

	if (!Item)
		return;

	if (!Item->GetCanInteract())
	{
		Item->DeactiveWidgetInteract();
		return;
	}

	if (GetActorLocation().Equals(LastUIUpdateLocation, 1.0f))
		return;
	LastUIUpdateLocation = GetActorLocation();

	FVector PlayerForward = GetActorForwardVector();
	FVector ToItem = (Item->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	float Alignment = FVector::DotProduct(PlayerForward, ToItem);

	if (Alignment < 0.5f)
	{
		Item->DeactiveWidgetInteract();
		return;
	}

	FAttachPointData BestPoint;

	bool bCanInteract = Item->GetClosestAttachPoint(GetActorLocation(), InteractDragRadius, BestPoint);

	if (bCanInteract)
	{
		Item->SetWidgetInteractLocation(BestPoint);

		FVector WorldPos = Item->GetActorTransform().TransformPosition(BestPoint.Position);
		DrawDebugSphere(GetWorld(), WorldPos, 15.f, 12, FColor::Green, false, 0.1f, 0, 2.f);
	}
	else
		Item->DeactiveWidgetInteract();
}

void ATPP_PlayerCharacter::ChangeBraceletColor(const ETPP_DrumCommands Command)
{
	const FLinearColor NewColor = DrumCommandsColorsDA->CommandColors.FindRef(Command);

	BraceletMaterialInstance->SetVectorParameterValue
	(
		FName("Color"),
		NewColor
	);
	DrumMaterialInstance->SetVectorParameterValue
	(
		FName("Color"),
		NewColor
	);

	CurrentBraceletColor = NewColor;
	BraceletFadeoutTimeline->PlayFromStart();
}

void ATPP_PlayerCharacter::HandleBraceletFadeOutProgress(float Output)
{
	const FLinearColor Color = FLinearColor::LerpUsingHSV
	(
		CurrentBraceletColor,
		StartingBraceletColor,
		Output
	);

	BraceletMaterialInstance->SetVectorParameterValue
	(
		FName("Color"),
		Color
	);
	DrumMaterialInstance->SetVectorParameterValue
	(
		FName("Color"),
		Color
	);
}

void ATPP_PlayerCharacter::OnBraceletFadeOutFinished()
{
	CurrentBraceletColor = StartingBraceletColor;
}

void ATPP_PlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (ATPP_PlayerController* PC = Cast<ATPP_PlayerController>(NewController))
		PlayerController = PC;
}

void ATPP_PlayerCharacter::OnPetFinished()
{
	if (!bIsPetting) return;
	bIsPetting = false;
	PlayerController->EnableInput(PlayerController);
	PlayerController->ToggleLocomotionMappingContext(true);
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		StopAnimMontage(PetMontage);
	}
	if (const UTPP_GameInstance* GI = Cast<UTPP_GameInstance>(GetGameInstance()))
    {
        UTPP_SaveGame* SaveGame = GI->GetSaveGameObject();
        if (SaveGame->bHasSeenPetCutScene)
            return;
		if (const ATPP_GameMode* GameMode = Cast<ATPP_GameMode>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->GetLevelSequencePlayer()->PlayPetSequence();
			SaveGame->bHasSeenPetCutScene = true;
			UGameplayStatics::SaveGameToSlot(SaveGame, SaveGame->SaveSlotName, SaveGame->UserIndex);
		}
    }
}

void ATPP_PlayerCharacter::OnOrbitStart()
{
	if (Drum->TrailVFX->IsVisible())
	{
		Drum->ToggleTrail(false);
		PlayerController->OnDrumDeactivated.Broadcast();
	}
	else
	{
		Drum->ToggleTrail(true);
		PlayerController->OnDrumActivated.Broadcast();
	}
}

void ATPP_PlayerCharacter::OnOrbitEnd()
{
	if (Drum->TrailVFX->IsVisible())
		Drum->ToggleTrail(false);
	else
		Drum->ToggleTrail(true);
}

void ATPP_PlayerCharacter::PlayDrumAcquisitionAnimation() const
{
	DrumOrbitObject->SetRelativeScale3D(FVector::OneVector);

	if (USkeletalMeshComponent* SkeletalMeshComponent = GetMesh())
	{
		if (UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance())
		{
			if (BodyAcquisitionMontage) { AnimInstance->Montage_Play(BodyAcquisitionMontage); }
		}
	}

	GetDrum()->PlayAcquisitionAnimation();
}

void ATPP_PlayerCharacter::ItemMediumInteract()
{
    if (!GetDetectedItem())
       return;

    FAttachPointData AttachPoint;

    // Check if the player is near the item's attach point.
    bool bNearAttach = GetDetectedItem()->GetClosestAttachPoint(
       GetActorLocation(),
       InteractDragRadius,
       AttachPoint
    );

    // If the player is too far from the attach point, display a debug message and exit.
    if (!bNearAttach)
    {
       GEngine->AddOnScreenDebugMessage(1, 1.5f, FColor::Red, "Too far from attach point");
       return;
    }

    // Disable the locomotion mapping context to prepare for interaction.
    PlayerController->ToggleLocomotionMappingContext(false);

    // Calculate the world position of the attach point.
    FVector AttachPointWorld = GetDetectedItem()->GetActorTransform().TransformPosition(AttachPoint.Position);

    // Get the rotation of the attach point.
    FRotator WarpRotation = AttachPoint.Rotation;

    // Calculate the required direction for the player to approach the attach point.
    FVector RequiredDirection = WarpRotation.Vector();
    RequiredDirection.Z = 0.f;
    RequiredDirection.Normalize();

    // Flatten the player's location to match the Z-coordinate of the attach point.
    FVector PlayerLocationFlat = GetActorLocation();
    PlayerLocationFlat.Z = AttachPointWorld.Z;

    // Calculate the vector from the attach point to the player.
    FVector ToPlayer = PlayerLocationFlat - AttachPointWorld;

    // Determine the distance along the approach direction and the lateral offset.
    float AlongApproach = FVector::DotProduct(ToPlayer, -RequiredDirection);
    FVector LateralOffset = ToPlayer - (-RequiredDirection * AlongApproach);

    // Check if too close
    float DragTooCloseRadius = DragApproachAcceptanceRadius * 0.8f;
    float DistanceXY = FVector::Dist2D(GetActorLocation(), AttachPointWorld);

    bool bTooClose = DistanceXY < DragTooCloseRadius;

    bool bNeedsPreApproach = (LateralOffset.Size() > DragApproachAcceptanceRadius * 0.5f) || bTooClose;

    // If a pre-approach phase is needed, calculate the pre-approach location.
    if (bNeedsPreApproach)
    {
       float BaseBackDistance = bTooClose ? (DragApproachAcceptanceRadius * 2.f) : DragApproachAcceptanceRadius * 1.5f;
       float BackDistance = FMath::Max(AlongApproach, BaseBackDistance);

       // Determine the pre-approach location by moving backward along the required direction.
       DragPreApproachLocation = AttachPointWorld + (-RequiredDirection * BackDistance);

       // Ensure the Z-coordinate of the pre-approach location matches the player's current Z-coordinate.
       DragPreApproachLocation.Z = GetActorLocation().Z;
    }

    //Too close
    if (DistanceXY < DragApproachAcceptanceRadius && !bNeedsPreApproach)
    {
       PlayerController->ToggleLocomotionMappingContext(true);
       if (GetDetectedItem() != nullptr)
       {
          SetActorRotation(AttachPoint.Rotation);
          DragAction();
       }
       return;
    }

    // Set the target location and rotation for the drag approach.
    DragApproachTargetLocation = AttachPointWorld;
    DragApproachTargetRotation = WarpRotation;

    // Disable orient-to-movement to avoid conflicts with movement input.
    GetCharacterMovement()->bOrientRotationToMovement = false;

    // Set flags for the pre-approach and drag approach phases.
    bIsInPreApproach = bNeedsPreApproach;
    bIsApproachingDragTarget = true;
    
    // Fail safe initializiation
    DragStuckTimer = 0.f;
    LastStuckCheckPosition = GetActorLocation();
}

void ATPP_PlayerCharacter::TickDragApproach(float DeltaSeconds)
{
    if (FVector::DistSquared(GetActorLocation(), LastStuckCheckPosition) < (StuckDistanceTolerance * StuckDistanceTolerance))
    {
        DragStuckTimer += DeltaSeconds;
    	
        if (DragStuckTimer >= DragStuckTimeThreshold)
        {
            bIsInPreApproach = false;
            bIsApproachingDragTarget = false;
            GetCharacterMovement()->bOrientRotationToMovement = true;
            PlayerController->ToggleLocomotionMappingContext(true);

#if WITH_EDITOR
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Controls Repristinated"));
#endif
            return;
        }
    }
    else
    {
        DragStuckTimer = 0.f;
        LastStuckCheckPosition = GetActorLocation();
    }
   
    if (bIsInPreApproach)
    {
       float DistToPreApproach =
          FVector::Dist2D(GetActorLocation(), DragPreApproachLocation);

       if (DistToPreApproach > DragPreApproachAcceptanceRadius)
       {
          FVector Direction =
             DragPreApproachLocation - GetActorLocation();

          Direction.Z = 0.f;
          Direction.Normalize();

          FRotator TargetRotation = Direction.Rotation();

          SetActorRotation(
             FMath::RInterpTo(
                GetActorRotation(),
                TargetRotation,
                DeltaSeconds,
                10.f));

          AddMovementInput(Direction, 1.f);

          return;
       }

       bIsInPreApproach = false;
    }

    // Move towardsAttach

    float DistanceXY =
       FVector::Dist2D(
          GetActorLocation(),
          DragApproachTargetLocation);

    FVector Direction =
       DragApproachTargetLocation - GetActorLocation();

    Direction.Z = 0.f;

    if (!Direction.IsNearlyZero())
    {
       Direction.Normalize();
    }

    FRotator NewRotation =
       FMath::RInterpTo(
          GetActorRotation(),
          DragApproachTargetRotation,
          DeltaSeconds,
          15.f);

    SetActorRotation(NewRotation);

    if (DistanceXY > DragApproachAcceptanceRadius)
    {
       AddMovementInput(Direction, 1.f);
       return;
    }

    //Reached Position

    const float RotationTolerance = 3.f;

    float YawDifference =
       FMath::Abs(
          FMath::FindDeltaAngleDegrees(
             GetActorRotation().Yaw,
             DragApproachTargetRotation.Yaw));

    if (YawDifference > RotationTolerance)
    {
       // KeepRotate
       return;
    }

    //SetActorLocation(DragApproachTargetLocation);
    //SetActorRotation(DragApproachTargetRotation);

    bIsInPreApproach = false;
    bIsApproachingDragTarget = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;

    PlayerController->ToggleLocomotionMappingContext(true);

#if WITH_EDITOR
    GEngine->AddOnScreenDebugMessage(
       10,
       2.f,
       FColor::Green,
       TEXT("[Reached] DragAction!"));
#endif

    if (GetDetectedItem())
    {
       FAttachPointData AttachPoint;
       bool bNearAttach = GetDetectedItem()->
          GetClosestAttachPoint(GetActorLocation(), InteractDragRadius, AttachPoint);
       if (!bNearAttach)
          return;
       SetActorRotation(AttachPoint.Rotation);
       DragAction();
    }
}

void ATPP_PlayerCharacter::Interact(TObjectPtr<UTPP_InteractAction_DA> Data)
{
	Super::Interact(MoveTemp(Data));
	
	if (Data)
	{
		if (Data->GetObjectInteractionType() == EObjectInteractionType::MockDrum)
		{
			OnInteractedWithMockDrum.Broadcast();
		}
	}
}

void ATPP_PlayerCharacter::SetActiveDrum(bool Active)
{
	bDrumActive = Active;
	Drum->TogglePylonRotation(Active);
	if (Active)
		OrbitTimeline.Play();
	else
	{
		OrbitTimeline.Reverse();
	}
}

void ATPP_PlayerCharacter::ComeHereDrumAction()
{
	if (!bCanUseComeHere)
		return;
	Drum->ComeHere(GetActorLocation());
}

void ATPP_PlayerCharacter::ComeHereDrumCompleted()
{
	if (!bCanUseComeHere)
		return;
	Drum->ComeHereCompleted();
	ChangeBraceletColor(ETPP_DrumCommands::ComeHere);
	bCanUseComeHere = false;
	GetWorld()->GetTimerManager().SetTimer(
		ComeHereTimerHandle,
		this,
		&ATPP_PlayerCharacter::SetCanUseComeHere,
		ComeHereCooldown,
		false
	);
}

void ATPP_PlayerCharacter::StopDrumAction()
{
	if (!bCanUseStop)
		return;
	bCanUseStop = false;
	Drum->Stop();
	ChangeBraceletColor(ETPP_DrumCommands::Stop);
	GetWorld()->GetTimerManager().SetTimer(
		StopTimerHandle,
		this,
		&ATPP_PlayerCharacter::SetCanUseStop,
		StopCooldown,
		false
	);
}

void ATPP_PlayerCharacter::PongoInteractAction()
{
	if (!bCanUsePongoInteract || !Drum->bInteractActive)
		return;
	Drum->Interact();
	ChangeBraceletColor(ETPP_DrumCommands::Interact);
	bCanUsePongoInteract = false;
	GetWorld()->GetTimerManager().SetTimer(
		PongoInteractTimerHandle,
		this,
		&ATPP_PlayerCharacter::SetCanUsePongoInteract,
		PongoInteractCooldown,
		false
	);
}

void ATPP_PlayerCharacter::GoThereAction()
{
	if (!bCanUseGoThere)
		return;
	if (!Drum->bGoThereActive)
		return;

	FVector MarkerDirection = GetActorForwardVector();
	FVector StartPosition = GetActorLocation() + (GetActorForwardVector() * SavedDistancePlayerDrum);
	Drum->GoThere(MarkerDirection, StartPosition, 50);
}

void ATPP_PlayerCharacter::GoThereActionStarted()
{
	if (!bCanUseGoThere)
		return;
	if (!Drum)
		return;
	if (!Drum->bGoThereActive)
		return;

	SetMaxSpeed(0.0f);
	SetMaxAcceleration(0.0f);
	
	GetWorld()->GetTimerManager().SetTimer(
		GoThereSoundStartTimerHandle,
		this,
		&ATPP_PlayerCharacter::ActivaGoThereUpdate,
		0.1f,
		false
	);
	Drum->GoThereStarted();
	Drum->ActiveGoThereStartedSound();
	ChangeBraceletColor(ETPP_DrumCommands::GoThere);
}

void ATPP_PlayerCharacter::GoThereActionCompleted()
{
	if (!bCanUseGoThere)
		return;
	if (!Drum)
		return;
	if (!Drum->bGoThereActive)
		return;
	if (!Drum->bGoThereInUse)
		return;

	ResetStartVelocity();
	Drum->DisableGoThereSound();
	Drum->GoThereCompleted();
	bCanUseGoThere = false;
	GetWorld()->GetTimerManager().SetTimer(
		GoThereTimerHandle,
		this,
		&ATPP_PlayerCharacter::SetCanUseGoThere,
		GoThereCooldown,
		false
	);
	//OnGoThereCompleted.Broadcast();
	ChangeBraceletColor(ETPP_DrumCommands::GoThere);
}

void ATPP_PlayerCharacter::GoThereActionCancelled()
{
	if (!bCanUseGoThere)
		return;
	if (!Drum)
		return;
	if (!Drum->bGoThereActive)
		return;
	if (!Drum->bGoThereInUse)
		return;

	bCanUseGoThere = false;
	GetWorld()->GetTimerManager().SetTimer(
		GoThereTimerHandle,
		this,
		&ATPP_PlayerCharacter::SetCanUseGoThere,
		GoThereCooldown,
		false
	);
	ResetStartVelocity();
	Drum->GoThereCancelled();
}

void ATPP_PlayerCharacter::PetPongo()
{
	if (bIsPetting)
		return;
	UAudioComponent* AudioComp;
	SoundComponent->PlaySound(TagPetPongo, GetActorLocation(), AudioComp);
	// Check if jumping
	if (GetCharacterMovement()->IsFalling())
		return;

	FHitResult HitResult = LineTrace(GetActorLocation(),
	                                 GetActorLocation() + GetActorForwardVector() * PetInteractionLenght);
#if WITH_EDITOR
	DrawDebugLine(GetWorld(),
	              GetActorLocation(),
	              GetActorLocation() + GetActorForwardVector() * PetInteractionLenght,
	              FColor::Blue,
	              false,
	              2.f);
#endif

	if (!IsValid(HitResult.GetActor()))
		return;

	if (!HitResult.GetActor()->ActorHasTag("Pongo"))
		return;

	UE_LOG(LogTemp, Warning, TEXT("Hit Actor %s"), *HitResult.GetActor()->GetName());

	if (ITPP_AIPettable* AIPettable = Cast<ITPP_AIPettable>(HitResult.GetActor()->GetInstigatorController()))
	{
		if (!AIPettable->CanBePet())
			return;
	}

	if (ATPP_Pongo* Pongo = Cast<ATPP_Pongo>(HitResult.GetActor()))
	{
		Pongo->OnPetFinished.RemoveDynamic(
			this,
			&ATPP_PlayerCharacter::OnPetFinished);

		Pongo->OnPetFinished.AddDynamic(
			this,
			&ATPP_PlayerCharacter::OnPetFinished);
	}
	UTPP_AIComms_OnPetEvent* PettingEvent = NewObject<UTPP_AIComms_OnPetEvent>();
	PettingEvent->Init(PetPercentValue);
	if (UTPP_AICommsProxySubsystem* Sub = GetWorld()->GetSubsystem<UTPP_AICommsProxySubsystem>())
	{
		Sub->NotifyAICommEvent(PettingEvent);
	}

	if (!PetMontage)
		return;

	// Stop movement
	PlayerController->ToggleLocomotionMappingContext(false);
	PlayerController->DisableInput(PlayerController);
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		bIsPetting = true;
		PlayAnimMontage(PetMontage);
	}
}

void ATPP_PlayerCharacter::SetDrum(ATPP_Drum* DrumReference)
{
	Drum = DrumReference;
	Drum->SetOwner(this);
	
	Drum->OnCancelledFinished.AddDynamic(this, &ATPP_PlayerCharacter::SetActiveDrum);
	Drum->OnCancelledFinished.AddDynamic(this, &ATPP_PlayerCharacter::PlayDrumStateSound);

	DrumMaterialInstance = Drum->MaterialInstances[0];

	SavedScaleOrbitObject = DrumOrbitObject->GetRelativeScale3D();

	const FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	Drum->AttachToComponent(DrumOrbitObject, AttachRules);
}

void ATPP_PlayerCharacter::PlayDrumStateSound(bool active)
{
	if (active)
	{
		if (AudioComUnPressed) { AudioComUnPressed->FadeOut(DurationFadeOutDrum, VolumeFadeOutDrum); }
		FTPP_AudioParameter Parameter = FTPP_AudioParameter();
		Parameter.Int = 0;
		SoundComponent->PlaySound(TagBongoState, GetActorLocation(), AudioComPressed, Parameter);
	}
	else
	{
		if (AudioComPressed) { AudioComPressed->FadeOut(DurationFadeOutDrum, VolumeFadeOutDrum); }
		FTPP_AudioParameter Parameter = FTPP_AudioParameter();
		Parameter.Int = 1;
		SoundComponent->PlaySound(TagBongoState, GetActorLocation(), AudioComPressed, Parameter);
	}
}

void ATPP_PlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	bIsCrouching = true;
	SetSizeCapsule(CrouchRadius, CrouchHalfHeight);
}

void ATPP_PlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	bJustLanded = true;
}

bool ATPP_PlayerCharacter::CanJump()
{
	if (GetCharacterMovement()->IsFalling() || bIsCrouching)
		return false;
	Jump();
	SoundComponent->PlaySound(TagJumpLand, GetActorLocation(), AudioCompJump);
	if (GetIsDragging()) { Interact(GetInteractableItem()->InteractWith()); }
	bJustLanded = false;
	return true;
}

void ATPP_PlayerCharacter::ActivateWidget(FVector PongoLocation)
{
	WidgetComp->SetVisibility(true);
	PongoLocation.Z += ZOffetWidget;
	WidgetComp->SetWorldLocation(PongoLocation);
}

void ATPP_PlayerCharacter::DeactivateWidget() { WidgetComp->SetVisibility(false); }

void ATPP_PlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	bIsCrouching = false;
	SetSizeCapsule(SavedStartRadius, SavedStartHalfHeight);
}


void ATPP_PlayerCharacter::CheckBackwardBlock()
{
	if (!GetCharacterMovement())
		return;

	FVector Velocity = GetCharacterMovement()->Velocity;
	FVector Forward = GetActorForwardVector();
	bool bIsMovingBackward = FVector::DotProduct(Velocity, Forward) < -5.0f;

	if (!bIsMovingBackward) { return; }
	FVector CheckLocation = GetActorLocation();
	CheckLocation.Z += ZOffestDragCheckBox;


	CheckLocation += (Forward * -1.f * ForwardOffetDragCheckBox);
	FVector BoxExtents = FVector(XSizeDragCheckBox + 20, YSizeDragCheckBox, ZSizeDragCheckBox);

	FCollisionQueryParams ParamsCollision;
	ParamsCollision.AddIgnoredActor(this);
	ParamsCollision.AddIgnoredActor(GetInteractableItem());

	bool bIsBlocked = GetWorld()->OverlapBlockingTestByChannel(
		CheckLocation,
		GetActorRotation().Quaternion(),
		ECC_WorldStatic,
		FCollisionShape::MakeBox(BoxExtents),
		ParamsCollision
	);
	DrawDebugBox(
		GetWorld(),
		CheckLocation,
		BoxExtents,
		GetActorRotation().Quaternion(),
		FColor::Red,
		false,
		0.1f
	);
	if (bIsBlocked)
	{
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		GetCharacterMovement()->StopMovementImmediately();
		bHasBlockedBackMovement = true;

#if WITH_EDITOR

		DrawDebugBox(
			GetWorld(),
			CheckLocation,
			BoxExtents,
			GetActorRotation().Quaternion(),
			FColor::Red,
			false,
			0.1f
		);

		GEngine->AddOnScreenDebugMessage(
			20,
			0.1f,
			FColor::Red,
			TEXT("[BACKWARD BLOCK] Blocked!")
		);
#endif
	}
	else
	{
		bHasBlockedBackMovement = false;
	}
}

void ATPP_PlayerCharacter::CheckDragBlock()
{
	if (!GetCharacterMovement())
		return;

	if (!GetInteractableItem())
		return;

	FVector Velocity = GetCharacterMovement()->Velocity;
	Velocity.Z = 0.f;

	FVector Forward = GetActorForwardVector();

	bool bIsMovingFor = FVector::DotProduct(Velocity, Forward) > 0.f;

	if (!bIsMovingFor || Velocity.IsNearlyZero()) { return; }

	FVector MoveDirection = Forward;

	FVector ItemLocation = GetInteractableItem()->GetActorLocation();
	FVector PlayerLocation = GetActorLocation();

	FVector CheckLocation = FVector(ItemLocation.X, ItemLocation.Y, PlayerLocation.Z);
	CheckLocation.Z += ZOffestDragCheckBox;

	float SpawnDistance = XSizeDragCheckBox + 20;

	CheckLocation += (MoveDirection * SpawnDistance);

	FVector BoxExtents = FVector(XSizeDragCheckBox, YSizeDragCheckBox, ZSizeDragCheckBox);

	FCollisionQueryParams ParamsCollision;
	ParamsCollision.AddIgnoredActor(this);
	ParamsCollision.AddIgnoredActor(GetInteractableItem());

	bool bIsBlocked = GetWorld()->OverlapBlockingTestByChannel(
		CheckLocation,
		GetActorRotation().Quaternion(), 
		ECC_WorldStatic,
		FCollisionShape::MakeBox(BoxExtents),
		ParamsCollision
	);

#if WITH_EDITOR
	DrawDebugBox(GetWorld(), CheckLocation, BoxExtents, GetActorRotation().Quaternion(), FColor::Orange, false, 0.1f);
	if (bIsBlocked)
	{
		GEngine->AddOnScreenDebugMessage(21, 0.1f, FColor::Orange, TEXT("[FORWARD BLOCK] Blocked!"));
	}
#endif

	if (bIsBlocked)
	{
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		GetCharacterMovement()->StopMovementImmediately();
		bHasBlockedForMovement = true;
		bHasBlockedBackMovement = false;
	}
	else
	{
		bHasBlockedForMovement = false;
	}
}

void ATPP_PlayerCharacter::HandleOrbitProgress(float Value)
{
	const float Angle = FMath::Lerp(0.f, 180.f, Value);

	DrumPivot->SetRelativeRotation(FRotator(0.f, Angle, 0.f));

	const float ScaleValue = FMath::Lerp(SavedScaleOrbitObject.X, 1.f, Value);
	DrumOrbitObject->SetRelativeScale3D(FVector(ScaleValue));

	if (Angle > 175.f) { bDrum = true; }
	else { bDrum = false; }
}
