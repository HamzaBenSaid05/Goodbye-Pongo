#include "Item/Interactable/TPP_InteractableItem.h"

#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TPP_SoundComponent.h"
#include "Components/WidgetComponent.h"
#include "DataAsset/InteractActions/TPP_InteractAction_DA.h"
#include "EventProxies/TPP_AudioProxySubsystem.h"
#include "NavAreas/NavArea_Default.h"
#include "NavAreas/NavArea_Null.h"

class UTPP_AudioProxySubsystem;

TObjectPtr<class UTPP_InteractAction_DA> ATPP_InteractableItem::InteractWith()
{
	if (BCanInteract == true)
		return InteractData;
	else
		return nullptr;
}

FFaceComponents ATPP_InteractableItem::GetSceneComponentFromDir(ETPP_FaceDir Dir)
{
	FFaceComponents Result;

	switch (Dir)
	{
	case ETPP_FaceDir::Front:
		Result.FirstComponent = Front_L;
		Result.SecondComponent = Front_R;
		break;

	case ETPP_FaceDir::Back:
		Result.FirstComponent = Back_L;
		Result.SecondComponent = Back_R;
		break;

	case ETPP_FaceDir::Right:
		Result.FirstComponent = Right_L;
		Result.SecondComponent = Right_R;
		break;

	case ETPP_FaceDir::Left:
		Result.FirstComponent = Left_L;
		Result.SecondComponent = Left_R;
		break;
	default:
		break;
	}

	return Result;
}

ETPP_FaceDir ATPP_InteractableItem::GetFaceDir(AActor* TargetActor)
{
	const FVector CubeLoc = GetActorLocation();

	if (!TargetActor)
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
			                                 -1, 5.f, FColor::Red,
			                                 TEXT("Target Actor is Null"));
		}
#endif
		return ETPP_FaceDir::None;
	}

	const FVector PlayerLoc = TargetActor->GetActorLocation();
	const FVector DirToPlayer = (PlayerLoc - CubeLoc).GetSafeNormal();

	const FVector F = GetActorForwardVector();
	const FVector R = GetActorRightVector();

	const float FrontDot = FVector::DotProduct(F, DirToPlayer);
	const float BackDot = FVector::DotProduct(-F, DirToPlayer);
	const float RightDot = FVector::DotProduct(R, DirToPlayer);
	const float LeftDot = FVector::DotProduct(-R, DirToPlayer);

	float BestDot = FrontDot;
	ETPP_FaceDir BestDir = ETPP_FaceDir::Front;

	if (BackDot > BestDot)
	{
		BestDot = BackDot;
		BestDir = ETPP_FaceDir::Back;
	}

	if (RightDot > BestDot)
	{
		BestDot = RightDot;
		BestDir = ETPP_FaceDir::Right;
	}

	if (LeftDot > BestDot)
	{
		BestDot = LeftDot;
		BestDir = ETPP_FaceDir::Left;
	}

	return BestDir;
}

ATPP_InteractableItem::ATPP_InteractableItem()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	BoxNavMesh = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxNavMesh"));
	BoxNavMesh->SetupAttachment(Mesh);

	// Setting Boxes Nav Mesh
	BoxNavMesh->bDynamicObstacle = true;
	BoxNavMesh->SetAreaClassOverride(UNavArea_Null::StaticClass());

	Front_L = CreateDefaultSubobject<UArrowComponent>(TEXT("Front_L"));
	Front_L->SetupAttachment(Mesh);

	Front_R = CreateDefaultSubobject<UArrowComponent>(TEXT("Front_R"));
	Front_R->SetupAttachment(Mesh);

	Back_L = CreateDefaultSubobject<UArrowComponent>(TEXT("Back_L"));
	Back_L->SetupAttachment(Mesh);

	Back_R = CreateDefaultSubobject<UArrowComponent>(TEXT("Back_R"));
	Back_R->SetupAttachment(Mesh);

	Right_L = CreateDefaultSubobject<UArrowComponent>(TEXT("Right_L"));
	Right_L->SetupAttachment(Mesh);

	Right_R = CreateDefaultSubobject<UArrowComponent>(TEXT("Right_R"));
	Right_R->SetupAttachment(Mesh);

	Left_L = CreateDefaultSubobject<UArrowComponent>(TEXT("Left_L"));
	Left_L->SetupAttachment(Mesh);

	Left_R = CreateDefaultSubobject<UArrowComponent>(TEXT("Left_R"));
	Left_R->SetupAttachment(Mesh);

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	WidgetComp->SetupAttachment(RootComponent);

	WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComp->SetUsingAbsoluteRotation(true);
	WidgetComp->SetVisibility(false);
	WidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SoundComponent = CreateDefaultSubobject<UTPP_SoundComponent>(TEXT("SoundComponent"));
}

void ATPP_InteractableItem::BeginPlay()
{
	Super::BeginPlay();
	FVector InitLocation = GetActorLocation() + FVector(0, 0, StartFallOffset);
	SetActorLocation(InitLocation);
	SaveStartLocation();
	UpdateRotationToReach();
}

//void ATPP_InteractableItem::OnConstruction(const FTransform& Transform)
//{
//	Super::OnConstruction(Transform);
//	UpdateAutoGripTargets();
//}

void ATPP_InteractableItem::ResetStartLocation()
{
	SetItemCollision(false, ECollisionEnabled::Type::QueryOnly);
	this->SetActorLocation(StartLocation);
	this->SetActorRotation(StartRotation);
}

void ATPP_InteractableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (const FAttachPointData& Data : AttachPoints)
	{
		FVector WorldPos = GetActorTransform().TransformPosition(Data.Position);

		DrawDebugSphere(
		                GetWorld(),
		                WorldPos,
		                40.f,
		                12,
		                FColor::Blue,
		                false,
		                0.f
		               );

		DrawDebugDirectionalArrow(
		                          GetWorld(),
		                          WorldPos,
		                          WorldPos + Data.Rotation.Vector() * 80.f,
		                          40.f,
		                          FColor::Yellow,
		                          false,
		                          0.f
		                         );
	}
}

void ATPP_InteractableItem::PlayDropSound()
{
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = 0;
	SoundComponent->PlaySound(TagItem,GetActorLocation(),AudioCompDrop,Parameter);
}

void ATPP_InteractableItem::PlayPushSound()
{
	if(AudioCompPush) return;
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = 1;
	SoundComponent->PlaySound(TagItem,GetActorLocation(),AudioCompPush,Parameter);
}

void ATPP_InteractableItem::DisablePushSound()
{
	if(!AudioCompPush) return;
	AudioCompPush->Stop();
	AudioCompPush = nullptr;
}

bool ATPP_InteractableItem::GetClosestAttachPoint(
	const FVector& PlayerLocation,
	float Radius,
	FAttachPointData& OutPoint)
{
	float BestDistance = Radius;

	bool bFound = false;

	for (const FAttachPointData& Data : AttachPoints)
	{
		FVector WorldPos = GetActorTransform().TransformPosition(Data.Position);

		float Dist = FVector::Dist(PlayerLocation, WorldPos);

		if (Dist < BestDistance)
		{
			BestDistance = Dist;
			OutPoint = Data;
			bFound = true;
		}
	}

	return bFound;
}

void ATPP_InteractableItem::SetItemCollision(bool SimulatedPhysics, ECollisionEnabled::Type TypeCollison)
{
	this->Mesh->SetSimulatePhysics(SimulatedPhysics);
	this->Mesh->SetCollisionEnabled(TypeCollison);
}

void ATPP_InteractableItem::SetWidgetInteractLocation(FAttachPointData SelectedAttachPointData)
{
	WidgetComp->SetVisibility(true);
	WidgetComp->SetWorldLocation(GetActorTransform().TransformPosition(SelectedAttachPointData.Position+FVector(0.0f,0.0f,WidgetOffsetZ)));
}

void ATPP_InteractableItem::DeactiveWidgetInteract()
{
	WidgetComp->SetVisibility(false);
}

void ATPP_InteractableItem::UpdateItemInteraction(const FVector& PlayerInput, const FVector& ForwardVector) {}

bool ATPP_InteractableItem::IsAIInteractionCompleted() const { return true; }

void ATPP_InteractableItem::EndAIInteraction()
{
	if (GetInteractData()->GetObjectInteractionType() == EObjectInteractionType::Large)
	{
		StartLocation = CoordinatesMediumPointInteraction;

		if (!bHasReachFinalPoint)
			bHasReachFinalPoint = true;
		else { bHasReachFinalPoint = false; }
	}
}

void ATPP_InteractableItem::UpdateRotationToReach()
{
	for (FAttachPointData& Data : AttachPoints)
	{
		if (Data.Position.IsNearlyZero())
		{
			float SnappedActorYaw = FMath::GridSnap(GetActorRotation().Yaw, 90.0f);
			Data.Rotation = FRotator(0.f, FRotator::ClampAxis(SnappedActorYaw), 0.f);
			continue;
		}

		float RelativeYaw = 0.0f;

		if (FMath::Abs(Data.Position.X) > FMath::Abs(Data.Position.Y))
			RelativeYaw = (Data.Position.X > 0) ? 180.0f : 0.0f;
		else
			RelativeYaw = (Data.Position.Y > 0) ? 270.0f : 90.0f;

		float WorldYaw = GetActorRotation().Yaw + RelativeYaw;

		float SnappedYaw = FMath::GridSnap(WorldYaw, 90.0f);

		Data.Rotation = FRotator(0.f, FRotator::ClampAxis(SnappedYaw), 0.f);
	}
}

void ATPP_InteractableItem::SetInteract(bool active) { BCanInteract = active; }

void ATPP_InteractableItem::DestroyItem() { Destroy(); }

bool ATPP_InteractableItem::GetHandSurfaceTransform(
	ETPP_FaceDir Dir,
	bool bLeftHand,
	FVector& OutLocation,
	FRotator& OutRotation) const
{
	FFaceComponents FaceComps = const_cast<ATPP_InteractableItem*>(this)->GetSceneComponentFromDir(Dir);
	UArrowComponent* Arrow = bLeftHand ? FaceComps.FirstComponent : FaceComps.SecondComponent;

	if (!Arrow)
		return false;

	OutLocation = Arrow->GetComponentLocation();
	OutRotation = Arrow->GetComponentRotation() + OffsetRotator;

	return true;
}

void ATPP_InteractableItem::SaveStartLocation()
{
	StartLocation = GetActorLocation();
	StartRotation = GetActorRotation();

	if (GetInteractData()!=nullptr &&  GetInteractData()->GetObjectInteractionType() == EObjectInteractionType::Large)
	{
		FinalPongoLocation = GetActorTransform().TransformPosition(LocationToReach);
		CoordinatesMediumPointInteraction = GetActorTransform().TransformPosition(NewStartDragPoint);
	}
}
