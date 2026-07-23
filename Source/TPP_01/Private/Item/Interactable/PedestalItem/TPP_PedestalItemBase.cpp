// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Interactable/PedestalItem/TPP_PedestalItemBase.h"

#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TPP_SoundComponent.h"
#include "DataAsset/InteractActions/TPP_Orb_DA.h"
#include "Item/Interactable/TPP_OrbItem.h"

// Sets default values
ATPP_PedestalItemBase::ATPP_PedestalItemBase()
{
	PrimaryActorTick.bCanEverTick = true;
	auto Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Scene);
	PickUpPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SnapZone"));
	PickUpPoint->SetupAttachment(GetRootComponent());

	DropBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DropBox"));
	DropBox->SetupAttachment(GetRootComponent());

	PickUpBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PickUpBox"));
	PickUpBox->SetupAttachment(GetRootComponent());
	
	Emitter = CreateDefaultSubobject<USceneComponent>(TEXT("Emitter"));
	Emitter->SetupAttachment(GetRootComponent());

	GetMesh()->SetupAttachment(GetRootComponent());
}

void ATPP_PedestalItemBase::BeginPlay()
{
	Super::BeginPlay();
	if(!DropBox)
		return;
	DropBox->OnComponentBeginOverlap.AddDynamic(
	                                            this,
	                                            &ATPP_PedestalItemBase::OnCollisionBeginOverlap
	                                           );
	if(Orb)
	{
		this->SetActorTickEnabled(true);
		FAttachmentTransformRules Rules(
		                                EAttachmentRule::SnapToTarget,
		                                EAttachmentRule::SnapToTarget,
		                                EAttachmentRule::SnapToTarget,
		                                true);
		Orb->AttachToComponent(PickUpPoint, Rules);
		if (!Orb->Tags.Contains("OnPedestal"))
		{
			Orb->Tags.Add(FName("OnPedestal"));
		}
	}
	else { this->SetActorTickEnabled(false); }
}

void ATPP_PedestalItemBase::SnapItem(ATPP_InteractableItem * Item)
{
	if(!Item || !bIsReactivable)
		return;

	Orb = Item;
	FAttachmentTransformRules Rules(
	                                EAttachmentRule::SnapToTarget,
	                                EAttachmentRule::SnapToTarget,
	                                EAttachmentRule::SnapToTarget,
	                                true);
	Orb->AttachToComponent(PickUpPoint, Rules);
	this->SetActorTickEnabled(true);
	if (!Orb->Tags.Contains("OnPedestal"))
	{
		Orb->Tags.Add(FName("OnPedestal"));
	}
	EnableObject();
}

void ATPP_PedestalItemBase::OnCollisionBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ATPP_InteractableItem * Item = Cast<ATPP_InteractableItem>(OtherActor);

	SnapItem(Item);
}

void ATPP_PedestalItemBase::EnableObject()
{
	if (AudioCompPedestal && AudioCompPedestal->IsPlaying())
	{
		AudioCompPedestal->Stop();
	}
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = 0;
	SoundComponent->PlaySound(TagPedestal, GetActorLocation(), AudioCompPedestal,Parameter,Emitter);
}

void ATPP_PedestalItemBase::DisableObject()
{
	if (AudioCompPedestal !=nullptr && AudioCompPedestal->IsPlaying())
	{
		AudioCompPedestal->Stop();
	}
	if (Orb->Tags.Contains(FName("OnPedestal")))
	{
		Orb->Tags.Remove(FName("OnPedestal"));
	}
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = 1;
	SoundComponent->PlaySound(TagPedestal, GetActorLocation(), AudioCompPedestal,Parameter,Emitter);
}

void ATPP_PedestalItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(Orb != nullptr)
	{
		if(!Orb->IsAttachedTo(this))
		{
			DisableObject();
			SetActorTickEnabled(false);
			Orb = nullptr;
		}
	}
}
