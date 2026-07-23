#include "Item/Hazard/PressurePlates/TPP_PressurePlate.h"
#include "Item/Hazard/HazardManagers/TPP_HazardManager.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TPP_SoundComponent.h"
#include "DataAsset/InteractActions/TPP_InteractAction_DA.h"
#include "Item/Interactable/TPP_InteractableItem.h"

// Sets default values
ATPP_PressurePlate::ATPP_PressurePlate()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetBoxExtent(FVector(50.f, 50.f, 10.f));
	CollisionBox->SetCollisionProfileName(TEXT("Trigger"));

	PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMesh"));
	PlateMesh->SetupAttachment(RootComponent);
	PlateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	SurfacePlateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SurfacePlateMesh"));
	SurfacePlateMesh->SetupAttachment(PlateMesh);
	SurfacePlateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SoundComponent = CreateDefaultSubobject<UTPP_SoundComponent>(TEXT("SoundComponent"));
}

void ATPP_PressurePlate::SetHazardManager(class ATPP_ElectricityManager* ElectricityManager)
{
	ElectricityManagerRef = ElectricityManager;
}

void ATPP_PressurePlate::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ATPP_PressurePlate::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ATPP_PressurePlate::OnOverlapEnd);
}

bool ATPP_PressurePlate::GetIsPressureEnough(TArray<AActor*>& Actors)
{
	if (Actors.Num() == 0)
		return false;

	if (Actors.Num() >= 1)
	{
		switch (PressurePlateType)
		{
		case EPressurePlateType::S:
			return true;
		case EPressurePlateType::L:
			for (AActor*& Actor : Actors)
			{
				if (Actor->ActorHasTag("Pongo"))
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("PongoIsOverIt"));
					return true;
				}

				ATPP_InteractableItem* InteractableItem = Cast<ATPP_InteractableItem>(Actor);
				if (InteractableItem)
				{
					if (InteractableItem->GetInteractData()->GetObjectInteractionType() == EObjectInteractionType::Medium ||
						InteractableItem->GetInteractData()->GetObjectInteractionType() == EObjectInteractionType::Large)
						return true;
				}
			}
			break;
		default:
			return false;
		}
	}

	return false;
}

void ATPP_PressurePlate::SwitchElectricity_Implementation()
{
	ElectricityManagerRef->OnSwitchActivationHandle(this);
}

void ATPP_PressurePlate::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (Cast<ATPP_InteractableItem>(OtherActor) || OtherActor->ActorHasTag("Player") || OtherActor->ActorHasTag("Pongo"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Pressure Plate Pressed!"));
			OverlappingActors.AddUnique(OtherActor);
			if (!bIsPlatePressed && GetIsPressureEnough(OverlappingActors))
			{
				bIsPlatePressed = true;
				PlayAnimation();
				if (AudioComp != nullptr)
				{
					AudioComp->Stop();
				}
				SoundComponent->PlaySound(TagPress, GetActorLocation(), AudioComp);
				IIPowerSwitch::Execute_SwitchElectricity(this);
			}
		}
	}
}

void ATPP_PressurePlate::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                      int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this)
	{
		if (OverlappingActors.Contains(OtherActor))
		{
			OverlappingActors.Remove(OtherActor);
			if (OverlappingActors.Num() == 0)
			{
				if (bIsPlatePressed)
				{
					bIsPlatePressed = false;
					PlayAnimation();
					if (AudioComp != nullptr)
					{
						AudioComp->Stop();
					}
					SoundComponent->PlaySound(TagUnPress, GetActorLocation(), AudioComp);
					IIPowerSwitch::Execute_SwitchElectricity(this);
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Pressure Plate Off!"));
				}
			}
		}
	}
}
