#include "Characters/TPP_CharacterBase.h"

#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "DataAsset/InteractActions/TPP_InteractAction_DA.h"
#include "Item/Interactable/TPP_InteractableItem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/TPP_DeathComponent.h"
#include "Item/Interactable/TPP_ItemSwitch.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

ATPP_CharacterBase::ATPP_CharacterBase()
{
	PickupPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PickupPoint"));
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	SoundComponent = CreateDefaultSubobject<UTPP_SoundComponent>(TEXT("SoundComponent"));
	PickupPoint->SetupAttachment(GetCapsuleComponent());

	DeathComp = CreateDefaultSubobject<UTPP_DeathComponent>(TEXT("Death Component"));
}

void ATPP_CharacterBase::Interact(TObjectPtr<class UTPP_InteractAction_DA> Data)
{
	if (Data)
	{
		switch (Data->GetObjectInteractionType())
		{
		case EObjectInteractionType::Small:
			ItemSmallInteract();
			break;
		case EObjectInteractionType::Medium:
			ItemMediumInteract();
			break;
		case EObjectInteractionType::Large:
			ItemLargeInteract();
			break;
		case EObjectInteractionType::ElectricSwitch:
			ElectricSwitchInteract();
			break;
		case EObjectInteractionType::Orb:
			ItemOrbInteract();
			break;
		case EObjectInteractionType::CuriosityObject:
			CuriosityObjectInteract();
			break;
		default:
			break;
		}
	}
	else
	{
		if (GetInteractableItem() == nullptr)
			return;
		EndInteract(GetInteractableItem()->GetInteractData());
	}
}

void ATPP_CharacterBase::EndInteract(TObjectPtr<class UTPP_InteractAction_DA> Data)
{
	if (Data)
	{
		switch (Data->GetObjectInteractionType())
		{
		case EObjectInteractionType::Small:
			ItemSmallEndInteraction();
			break;
		case EObjectInteractionType::Medium:
			ItemMediumEndInteraction();
			break;
		case EObjectInteractionType::Large:
			ItemLargeEndInteraction();
			break;
		case EObjectInteractionType::ElectricSwitch:
			ElectricSwitchEndInteraction();
			break;
		case EObjectInteractionType::Orb:
			ItemOrbEndInteraction();
			break;
		case EObjectInteractionType::CuriosityObject:
			CuriosityObjectEndInteraction();
			break;
		default:
			break;
		}
	}
}

bool ATPP_CharacterBase::IsInteracting()
{
	return GetInteractableItem() != nullptr;
}

void ATPP_CharacterBase::ItemSmallInteract() { PickUpAction(); }

void ATPP_CharacterBase::ItemSmallEndInteraction() { DropAction(); }

void ATPP_CharacterBase::ItemMediumInteract() { PickUpAction(); }

void ATPP_CharacterBase::ItemMediumEndInteraction() { DropAction(); }

void ATPP_CharacterBase::ItemLargeInteract() { DragAction(); }

void ATPP_CharacterBase::ItemLargeEndInteraction() { EndDragAction(); }

void ATPP_CharacterBase::ItemOrbInteract()
{
	if (GetInteractableItem() == nullptr)
		PickUpAction();
	//else { SearchForPedestal(); }
}

void ATPP_CharacterBase::ItemOrbEndInteraction()
{
	//SearchForPedestal();
	DropAction();
}

void ATPP_CharacterBase::ElectricSwitchInteract()
{
	ActivateElectricSwitch();
}

void ATPP_CharacterBase::ElectricSwitchEndInteraction() { EndDragAction(); }

void ATPP_CharacterBase::CuriosityObjectInteract()
{
	PickUpAction();
	UAudioComponent* AudioComponent = nullptr;
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = 5;
	SoundComponent->PlaySound(TagInteraction,GetActorLocation(),AudioComponent,Parameter,GetRootComponent());
	GetInteractableItem()->DestroyItem();
}

void ATPP_CharacterBase::CuriosityObjectEndInteraction() {}

void ATPP_CharacterBase::Kill_Implementation(const bool bShouldSoftReset, const ECharacterDeath CharacterDeath, const EDeathCause DeathCause)
{
	if (DeathComp->bIsDying)
	{
		return;
	}

	DeathComp->bIsDying = true;
	
	UAudioComponent* AudioComp;
	SoundComponent->PlaySound(TagDeath,GetActorLocation(),AudioComp);
	if (GetController())
	{
		Execute_Kill(GetController(), bShouldSoftReset, CharacterDeath, DeathCause);
	}
	
	FDeathInfo DeathInfo;
	DeathInfo.bShouldSoftReset = bShouldSoftReset;
	DeathInfo.CharacterDeath = CharacterDeath;
	DeathInfo.DeathCause = DeathCause;
	DeathInfo.Mesh = GetMesh();

	
	DeathComp->Killed(DeathInfo);
}

void ATPP_CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	SavedVelocity = GetCharacterMovement()->MaxWalkSpeed;
	StartingMinWalkSpeed = GetCharacterMovement()->MinAnalogWalkSpeed;
	SavedAcceleration = GetCharacterMovement()->MaxAcceleration;
}

void ATPP_CharacterBase::FellOutOfWorld(const class UDamageType& dmgType)
{
	const ECharacterDeath DeadCharacter = ActorHasTag("Player") ? ECharacterDeath::PlayerDeath : ECharacterDeath::PongoDeath;
	Execute_Kill(this, false, DeadCharacter, EDeathCause::KillBox);
}

FHitResult ATPP_CharacterBase::LineTrace(const FVector& Start, const FVector& End)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (!GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params)) { return Hit; }

	return Hit;
}

FHitResult ATPP_CharacterBase::BoxTrace(const FVector& Start, const FVector& End, const FVector& Size, const FCollisionQueryParams& Params, ECollisionChannel TraceChannel) const
{
	FHitResult Hit;

	FCollisionShape BoxShape = FCollisionShape::MakeBox(Size);

	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		Start,
		End,
		FQuat::Identity,
		TraceChannel,
		BoxShape,
		Params
	);

	//DrawDebugBox(GetWorld(), Start, Size, bHit ? FColor::Red : FColor::Green, false, 0.f, 0);
	DrawDebugBox(GetWorld(), End, Size, bHit ? FColor::Red : FColor::Green, false, 0.f, 0);

	return Hit;
}

void ATPP_CharacterBase::VisualizeUIItemInteraction() {}

void ATPP_CharacterBase::PickUpAction()
{
	if (GetDetectedItem() == nullptr)
		return;
	if (this->GetCharacterMovement()->IsFalling())
		return;
	FVector Min, Max;
	GetDetectedItem()->GetMesh()->GetLocalBounds(Min, Max);
	ItemBounds = Max;
	FVector ItemLocation = GetDetectedItem()->GetActorLocation();
	float CheckHeight = GetActorLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	if (ItemLocation.Z < CheckHeight)
		return;
	SetInteractableItem(GetDetectedItem());
	GetInteractableItem()->DeactiveWidgetInteract();

	if (PhysicsHandle->GrabbedComponent) { PhysicsHandle->ReleaseComponent(); }

	GetInteractableItem()->GetMesh()->SetSimulatePhysics(false);
	ItemScale = GetInteractableItem()->GetActorScale();
	GetDetectedItem()->GetBoxNavMesh()->bDynamicObstacle = false;
	GetInteractableItem()->SetInteract(false);
	FAttachmentTransformRules Rules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		true);

	UStaticMeshComponent* ItemMesh = GetInteractableItem()->GetMesh();
	ItemExtent = ItemMesh->Bounds.BoxExtent;
	ItemMesh->SetCollisionResponseToChannel(ClimbableChannel, ECR_Ignore);

	FVector NewRelativeLocation = PickupPoint->GetRelativeLocation();
	NewRelativeLocation.X = ItemBounds.X + OffsetPickUp;

	PickupPoint->SetRelativeLocation(NewRelativeLocation);

	GetInteractableItem()->AttachToComponent(PickupPoint, Rules, SocketName);

	UAudioComponent* AudioComp = nullptr;
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = 0;
	SoundComponent->PlaySound(TagInteraction, GetActorLocation(), AudioComp,Parameter,GetRootComponent());
	// Speed
	SetMaxSpeed(PickUpVelocity);
	SetMaxAcceleration(PickUpAcceleration);

	if (!GetInteractableItem()->Tags.Contains("Pickup"))
	{
		GetInteractableItem()->Tags.Add(FName("PickUp"));
	}
	// Collision Enable
	GetInteractableItem()->GetMesh()->SetCollisionEnabled(CollisionState);
	GetInteractableItem()->SetActorScale3D(ItemScale);
}

void ATPP_CharacterBase::DropAction()
{
	SetMaxSpeed(SavedVelocity);
	SetMaxAcceleration(SavedAcceleration);

	if (!GetInteractableItem())
		return;

	AActor* Item = GetInteractableItem();

	ECollisionEnabled::Type CollisionType = ECollisionEnabled::Type::QueryOnly;
	bool Detach = true;

	FVector BaseLocation = GetActorLocation();
	FVector Forward = GetActorForwardVector();
	FVector Right = GetActorRightVector();

	FVector Up = GetActorUpVector();

	// =====CAPSULE_INFO=====
	float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	BaseLocation.Z = (BaseLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) + ItemBounds.Z;
	// Distance
	float ForwardDistance = CapsuleRadius + ItemBounds.X + OffsetDrop;

	FVector DropPosition = FVector::ZeroVector;
	FRotator DropRotation = GetActorRotation();

	bool bFoundValidSpot = false;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetInteractableItem());

	TArray<FVector> Directions = { Forward, Right, -Right };

	for (const FVector& Dir : Directions)
	{
		FVector HorizontalStart =
			BaseLocation + Dir * (CapsuleRadius + 5.0f);
		FVector HorizontalEnd =
			BaseLocation + Dir * ForwardDistance;
		FHitResult SweepHit = BoxTrace(HorizontalStart, HorizontalEnd, ItemBounds, Params);

		if (SweepHit.GetActor() == nullptr)
		{
			//====CHECK DROP ACTOR GROUND=====
			FVector GroundStart = HorizontalEnd;
			FVector GroundEnd = GroundStart - Up * MaxFallDistance;

			FHitResult GroundHit;

			GroundHit = LineTrace(GroundStart, GroundEnd);

			if (GroundHit.GetActor() != nullptr)
			{
				//====CHECK TAG=====
				if (GroundHit.GetActor()->ActorHasTag(NoDropTag)) { return; }
				if (GroundHit.GetActor()->ActorHasTag(Attachable))
				{
					UStaticMeshComponent* HitMesh = Cast<UStaticMeshComponent>(GroundHit.Component.Get());
					FAttachmentTransformRules Rules(
						EAttachmentRule::SnapToTarget,
						EAttachmentRule::SnapToTarget,
						EAttachmentRule::SnapToTarget,
						true);
					GetInteractableItem()->AttachToComponent(HitMesh, Rules);

					CollisionType = ECollisionEnabled::Type::QueryAndPhysics;
					Detach = false;
				}

				DropPosition = GroundHit.ImpactPoint + FVector(0, 0, ItemBounds.Z);
				//====CHECK DROP ACTOR GROUND SIZE=====

				float GroundOffset = 5;
				FVector EndGround = FVector(DropPosition.X, DropPosition.Y, DropPosition.Z + GroundOffset);
				FHitResult SweepGroundHit = BoxTrace(EndGround, EndGround, ItemBounds, Params);
				DrawDebugBox(GetWorld(), EndGround, ItemBounds, FColor::Red, false, 5);

				if (SweepGroundHit.GetActor() == nullptr || SweepGroundHit.GetActor() == InteractableItem)
				{
					bFoundValidSpot = true;
					break;
				}
			}
		}
	}

	// =====FALLBACK=====
	bool bSimulatePhysics = false;
	if (!bFoundValidSpot)
		return;
	if (GetInteractableItem()->Tags.Contains(FName("Pickup")))
	{
		GetInteractableItem()->Tags.Remove(FName("Pickup"));
	}

	// =====DROP=====
	if (Detach)
	{
		FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);
		OnItemDetached(GetInteractableItem());
		Item->DetachFromActor(Rules);
	}
	Item->SetActorRotation(DropRotation);
	DropPosition.Z += GetInteractableItem()->StartFallOffset;

	GetInteractableItem()->SetInteract(true);

	Item->SetActorLocation(DropPosition);

	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = 1;
	SoundComponent->PlaySound(TagInteraction, GetActorLocation(), AudioCompDrop, Parameter,GetRootComponent());

	GetInteractableItem()->PlayDropSound();
	GetInteractableItem()->GetMesh()->SetSimulatePhysics(bSimulatePhysics);
	GetInteractableItem()->GetMesh()->SetCollisionEnabled(CollisionType);
	GetInteractableItem()->GetMesh()->SetCollisionResponseToChannel(ClimbableChannel, ECR_Block);
	GetInteractableItem()->UpdateRotationToReach();
	SetInteractableItem(nullptr);
}

void ATPP_CharacterBase::OnItemDetached(ATPP_InteractableItem* Item) {}

FRotator ATPP_CharacterBase::GetDropRotation()
{
    if (!GetInteractableItem())
       return GetActorRotation();

    AActor* Item = GetInteractableItem();

    FVector BaseLocation = GetActorLocation();
    FVector Forward = GetActorForwardVector();
    FVector Right = GetActorRightVector();
    FVector Up = GetActorUpVector();

    float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
    BaseLocation.Z = (BaseLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) + ItemBounds.Z;

    float ForwardDistance = CapsuleRadius + ItemBounds.X + OffsetDrop;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(Item);

    TArray<FVector> Directions = { Forward, Right, -Right };

    for (const FVector& Dir : Directions)
    {
       FVector HorizontalStart = BaseLocation + Dir * (CapsuleRadius + 5.0f);
       FVector HorizontalEnd   = BaseLocation + Dir * ForwardDistance;

       FHitResult SweepHit = BoxTrace(HorizontalStart, HorizontalEnd, ItemBounds, Params);

       if (SweepHit.GetActor() == nullptr)
       {
          FVector GroundStart = HorizontalEnd;
          FVector GroundEnd   = GroundStart - Up * MaxFallDistance;

          FHitResult GroundHit = LineTrace(GroundStart, GroundEnd);

          if (GroundHit.GetActor() && !GroundHit.GetActor()->ActorHasTag(NoDropTag))
          {
           
             FRotator ResultRotation = Dir.Rotation();
             float SnappedYaw = FMath::GridSnap(ResultRotation.Yaw, 90.0f);
             
             ResultRotation.Yaw = SnappedYaw;
             ResultRotation.Pitch = 0.f;
             ResultRotation.Roll = 0.f;

             return ResultRotation;
          }
       }
    }
	
    FRotator FallbackRot = GetActorRotation();
    FallbackRot.Yaw = FMath::GridSnap(FallbackRot.Yaw, 90.0f);
    FallbackRot.Pitch = 0.f;
    FallbackRot.Roll = 0.f;
    return FallbackRot;
}
void ATPP_CharacterBase::ActivateElectricSwitch()
{
	if (GetDetectedItem() != nullptr)
	{
		IIPowerSwitch::Execute_SwitchElectricity(GetDetectedItem());
	}
}

void ATPP_CharacterBase::SetMaxSpeed(float MaxSpeed) { GetCharacterMovement()->MaxWalkSpeed = MaxSpeed; }

void ATPP_CharacterBase::SetMaxAcceleration(float Acceleration) const { GetCharacterMovement()->MaxAcceleration = Acceleration; }

void ATPP_CharacterBase::SearchForPedestal()
{
	FVector Start = GetActorLocation();
	FVector End = Start;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	TArray<FHitResult> Hits;

	bool bHit = GetWorld()->SweepMultiByChannel(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(SphereSearchRadius),
		Params
	);

	for (auto& Hit : Hits)
	{
		AActor* Actor = Hit.GetActor();
		ATPP_InteractableItem* Item = Cast<ATPP_InteractableItem>(Actor);
		if (!Item)
			return;
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow,TEXT("SearchStarted")); }
		if (Item->GetInteractData()->GetObjectInteractionType() == EObjectInteractionType::Pedestal)
		{
			//Do Somethings
			if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow,TEXT("FoundPedestal")); }
		}
	}
}

void ATPP_CharacterBase::DragAction()
{
	if (!bIsDragging)
	{
		if (GetDetectedItem())
			SetInteractableItem(GetDetectedItem());

		if (!GetInteractableItem())
			return;

		if (GetCharacterMovement()->bWantsToCrouch)
		{
			UnCrouch();
			GetCharacterMovement()->bWantsToCrouch = false;
		}
		
		GetInteractableItem()->SetInteract(false);

		UPrimitiveComponent* ItemMesh = GetInteractableItem()->GetMesh();
		DragRotation = GetInteractableItem()->GetActorRotation();
		GetInteractableItem()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(false);

		DragOffset = GetInteractableItem()->GetActorLocation() - GetActorLocation();

		SavedDragHorizontalOffset = FVector(
			DragOffset.X,
			DragOffset.Y,
			0.f
		);

		SavedDragWorldZ = GetInteractableItem()->GetActorLocation().Z;

		if (PhysicsHandle)
		{
			PhysicsHandle->GrabComponentAtLocationWithRotation(
				ItemMesh,
				NAME_None,
				ItemMesh->GetComponentLocation(),
				ItemMesh->GetComponentRotation()
			);
		}
		
		SavedDragHorizontalOffset =
		FVector(DragOffset.X, DragOffset.Y, 0.f) +
		GetActorForwardVector() * ForwardDragOffset;
		FVector TargetLocation =
			GetActorLocation() + SavedDragHorizontalOffset;

		TargetLocation.Z = SavedDragWorldZ;

		PhysicsHandle->SetTargetLocationAndRotation(
			TargetLocation,
			DragRotation
		);

		ItemMesh->BodyInstance.bLockXRotation = true;
		ItemMesh->BodyInstance.bLockYRotation = true;
		ItemMesh->BodyInstance.bLockZRotation = true;

		ItemMesh->BodyInstance.bLockXTranslation = true;
		ItemMesh->BodyInstance.bLockYTranslation = true;
		ItemMesh->BodyInstance.bLockZTranslation = true;

		FVector PlayerForward = GetActorForwardVector();

		float DotX = FMath::Abs(FVector::DotProduct(PlayerForward, GetInteractableItem()->GetActorRightVector()));
		float DotY = FMath::Abs(FVector::DotProduct(PlayerForward, GetInteractableItem()->GetActorForwardVector()));
		float DotZ = FMath::Abs(FVector::DotProduct(PlayerForward, GetInteractableItem()->GetActorUpVector()));

		if (DotX > DotY && DotX > DotZ)
			ItemMesh->BodyInstance.bLockXTranslation = false;
		else if (DotY > DotX && DotY > DotZ)
			ItemMesh->BodyInstance.bLockYTranslation = false;
		else
			ItemMesh->BodyInstance.bLockZTranslation = false;

		bIsDragging = true;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		SetMaxSpeed(DragVelocity);
	}
	else { EndDragAction(); }
}

void ATPP_CharacterBase::SetSizeCapsule(float& NewRadius, float& NewHalfHeight)
{
	GetCapsuleComponent()->SetCapsuleSize(NewRadius, NewHalfHeight, true);
	FVector NewPositionMesh = FVector(0.0f, 0.0f, -NewHalfHeight);
	GetMesh()->SetRelativeLocation(NewPositionMesh);
}

void ATPP_CharacterBase::EndDragAction()
{
	if (!bIsDragging || !GetInteractableItem())
		return;

	GetInteractableItem()->SetInteract(true);
	UPrimitiveComponent* ItemMesh = GetInteractableItem()->GetMesh();

	if (PhysicsHandle && PhysicsHandle->GrabbedComponent) { PhysicsHandle->ReleaseComponent(); }

	if (GetInteractableItem()->bHasToActivateGravity)
	{
		GetInteractableItem()->GetMesh()->SetSimulatePhysics(true);
		GetInteractableItem()->GetMesh()->SetEnableGravity(true);
	}
	else { ItemMesh->SetSimulatePhysics(false); }

	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Block);

	ItemMesh->BodyInstance.bLockXRotation = false;
	ItemMesh->BodyInstance.bLockYRotation = false;
	ItemMesh->BodyInstance.bLockZRotation = false;

	ItemMesh->BodyInstance.bLockXTranslation = false;
	ItemMesh->BodyInstance.bLockYTranslation = false;
	ItemMesh->BodyInstance.bLockZTranslation = false;

	GetInteractableItem()->DisablePushSound();

	GetInteractableItem()->SetInteract(true);
	GetInteractableItem()->UpdateRotationToReach();
	FVector EndLocation = GetInteractableItem()->GetActorLocation();
	EndLocation.Z -= DragZOffset;
	GetInteractableItem()->SetActorLocation(EndLocation);
	bIsDragging = false;
	SetInteractableItem(nullptr);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bHasBlockedBackMovement = false;
	bHasBlockedForMovement = false;
	SetMaxSpeed(SavedVelocity);
}

void ATPP_CharacterBase::PossessedBy(AController* NewController) { Super::PossessedBy(NewController); }

void ATPP_CharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector Size = FVector(SizeInteractX, SizeInteractY, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * SizeInteractZ);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	TArray<AActor*> ExtraIgnored;
	GetBoxTraceIgnoredActors(ExtraIgnored);
	for (AActor* Actor : ExtraIgnored)
	{
		if (Actor) Params.AddIgnoredActor(Actor);
	}
	
	FHitResult Hit = BoxTrace(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * ForwardOffset, Size, Params,BoxChannel);
	if (Hit.GetActor() == nullptr)
	{
		if (GetDetectedItem() != nullptr && bIsDragging)
		{
			GetDetectedItem()->DeactiveWidgetInteract();
			EndInteract(GetDetectedItem()->GetInteractData());
		}

		if (GetInteractableItem() != nullptr && bIsDragging)
		{
			GetInteractableItem()->DeactiveWidgetInteract();
			EndInteract(GetInteractableItem()->GetInteractData());
		}

		if (GetDetectedItem() != nullptr)
		{
			GetDetectedItem()->DeactiveWidgetInteract();
		}

		if (bIsDragging) { EndDragAction(); }

		SetDetectedItem(nullptr);
	}
	else { SetDetectedItem(Cast<ATPP_InteractableItem>(Hit.GetActor())); }

	if (GetDetectedItem())
		VisualizeUIItemInteraction();

	if (!bIsDragging || !GetInteractableItem() || !PhysicsHandle)
		return;
	if (bIsDragging && GetInteractableItem() && PhysicsHandle)
	{
		FVector Velocity = GetCharacterMovement()->Velocity;
		FVector Forward = GetActorForwardVector();

		bool bIsMovingForward = FVector::DotProduct(Velocity, Forward) > 0.f;

		if (GetCharacterMovement()->Velocity != FVector::ZeroVector)
		{
			if (!bIsMovingForward)
			{
				bHasBlockedForMovement = false;
				CheckBackwardBlock();
			}
			else
			{
				bHasBlockedBackMovement = false;
				CheckDragBlock();
			}
		}
	
		if (GetCharacterMovement()->Velocity.SizeSquared() > 0.0f)
		{
			if (GetInteractableItem()->AudioCompPush == nullptr)
			{
				if (AudioCompPush == nullptr)
				{
					FTPP_AudioParameter Parameter = FTPP_AudioParameter();
					Parameter.Int = 2;
					SoundComponent->PlaySound(TagInteraction, GetActorLocation(), AudioCompPush,Parameter);
				}
				GetInteractableItem()->PlayPushSound();
			}
		}
		else
		{
			if (AudioCompPush != nullptr)
			{
				AudioCompPush->Stop();
				AudioCompPush = nullptr;
			}
			GetInteractableItem()->DisablePushSound();
		}

		FVector TargetLocation = GetActorLocation() + SavedDragHorizontalOffset;
		TargetLocation.Z = SavedDragWorldZ + DragZOffset;
		PhysicsHandle->SetTargetLocation(TargetLocation);
	}
}
void ATPP_CharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	UAudioComponent* AudioComponent = nullptr;
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = 1;
	SoundComponent->PlaySound(TagJumpLand, GetActorLocation(), AudioComponent,Parameter,GetRootComponent());
}

void ATPP_CharacterBase::ResetStartVelocity()
{
	SetMaxSpeed(GetMaxSpeed());
	SetMaxAcceleration(GetMaxAcceleration());
}

void ATPP_CharacterBase::CheckBackwardBlock() {}

void ATPP_CharacterBase::CheckDragBlock(){}

