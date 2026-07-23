#include "Item/Circuits/TPP_ElectricCircuit.h"

#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Engine/DecalActor.h"
#include "Item/Hazard/PressurePlates/TPP_CircuitPressurePlate.h"

ATPP_ElectricCircuit::ATPP_ElectricCircuit()
{
	TimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("SphereMovingTimeline"));
	SoundComponent = CreateDefaultSubobject<UTPP_SoundComponent>(TEXT("SoundComponent"));
}

void ATPP_ElectricCircuit::StartCircuit()
{
	const int PPIndex = bGoingForward ? CurrentPP - 1 : CurrentPP;
	if (PressurePlates.IsValidIndex(PPIndex))
		PressurePlates[PPIndex]->TogglePlateMeshGraphics(false);

	TimelineComp->PlayFromStart();
}

void ATPP_ElectricCircuit::BeginPlay()
{
	Super::BeginPlay();

	SphereStartingLocation = EnergySphere->GetActorLocation();
	CurrentRail = 0;
	CurrentPP = -1;
	NumPlates = CircuitPressurePlates.Num();
	bGoingForward = true;

	if (SphereMovingCurve)
	{
		FOnTimelineFloat TimelineUpdate;
		TimelineUpdate.BindDynamic(this, &ATPP_ElectricCircuit::HandleTimelineUpdate);

		FOnTimelineEvent TimelineStart;
		TimelineStart.BindDynamic(this, &ATPP_ElectricCircuit::HandleTimelineStart);

		FOnTimelineEvent TimelineEnd;
		TimelineEnd.BindDynamic(this, &ATPP_ElectricCircuit::HandleTimelineEnd);

		TimelineComp->AddInterpFloat(SphereMovingCurve.Get(), MoveTemp(TimelineUpdate));
		TimelineComp->AddEvent(0, MoveTemp(TimelineStart));
		TimelineComp->AddEvent(TimelineComp->GetTimelineLength() - RadiusOffset, MoveTemp(TimelineEnd));
	}

	for (int i = 0; i < NumPlates; i++)
	{
		if (CircuitPressurePlates.Find(i)->PressurePlate)
		{
			CircuitPressurePlates.Find(i)->PressurePlate->OnToggled.AddDynamic(this, &ATPP_ElectricCircuit::HandlePressurePlateToggled);
			PressurePlates.Add(CircuitPressurePlates.Find(i)->PressurePlate.Get());
		}
	}
}

void ATPP_ElectricCircuit::HandleTimelineUpdate(float Output)
{
	if (CurrentRailMatInst)
	{
		float Ratio = bGoingForward ? Output : 1 - Output;
		CurrentRailMatInst->SetScalarParameterValue("Gradient", Ratio);
	}
}

void ATPP_ElectricCircuit::HandleTimelineStart()
{
	if (ADecalActor* TempDecal = CircuitPressurePlates.Find(CurrentPP)->Rails[CurrentRail])
	{
		if (UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(TempDecal->GetDecalMaterial()))
		{
			CurrentRailMatInst = MID;
		}
		else
		{
			CurrentRailMatInst = TempDecal->CreateDynamicMaterialInstance();
		}
	}

	EnergySphere.Get()->GetRootComponent()->SetVisibility(false);
}

void ATPP_ElectricCircuit::HandleTimelineEnd()
{
	if (!CircuitPressurePlates.Find(CurrentPP))
	{
		return;
	}

	const int EndRailIndex = bGoingForward ? CircuitPressurePlates.Find(CurrentPP)->Rails.Num() : -1;

	CurrentRail += bGoingForward ? 1 : -1;

	// Finished cycling through the rails in this plate
	if (CurrentRail == EndRailIndex)
	{
		GoToNextPlate();
	}
	else
	{
		TimelineComp->PlayFromStart();
	}
}

void ATPP_ElectricCircuit::HandlePressurePlateToggled(ATPP_CircuitPressurePlate* PressurePlate)
{
	const int32 ToggledPP = PressurePlates.IndexOfByKey(PressurePlate);
	if (PressurePlate->bIsActivated)
	{
		if (ToggledPP == CurrentPP + 1)
		{
			// Advancing pressure plate and resetting rail index
			CurrentPP++;
			CurrentRail = 0;
			bGoingForward = true;
			StartCircuit();
		}
	}
	else
	{
		if (ToggledPP == CurrentPP)
		{
			// Not advancing pressure plate, will do at the end of the "animation", setting the rail index at the last
			bGoingForward = false;
			CurrentRail = CircuitPressurePlates.Find(CurrentPP)->Rails.Num() - 1;
			StartCircuit();
		}
	}
}

void ATPP_ElectricCircuit::GoToNextPlate()
{
	CurrentRail = bGoingForward ? 0 : CircuitPressurePlates.Find(CurrentPP)->Rails.Num() - 1;
	CurrentPP = bGoingForward ? CurrentPP : CurrentPP - 1;

	// Moving EnergySphere
	if (CurrentPP == -1)
	{
		EnergySphere->SetActorLocation(SphereStartingLocation);
	}
	else
	{
		// SphereTargetLocation = PressurePlates[CurrentPP]->GetActorLocation();
		EnergySphere->AttachToActor(PressurePlates[CurrentPP], FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}

	// Toggling Plate material
	for (int i = 0; i < NumPlates; i++)
	{
		if (i == CurrentPP)
		{
			PressurePlates[i]->TogglePlateMeshGraphics(true);
		}
		else
		{
			PressurePlates[i]->TogglePlateMeshGraphics(false);
		}
	}

	// Checking whether we should keep traversing the circuit
	if (bGoingForward && PressurePlates.IsValidIndex(CurrentPP + 1))
	{
		UAudioComponent* AudioComponent = nullptr;
		FTPP_AudioParameter Parameter = FTPP_AudioParameter();
		Parameter.Int = 16;
		SoundComponent->PlaySound(TagEletricCircuit, PressurePlates[CurrentPP]->GetActorLocation(), AudioComponent, Parameter, PressurePlates[CurrentPP]->GetRootComponent());
		GEngine->AddOnScreenDebugMessage(-1,1,FColor::Green,"Porco dio");
		if (PressurePlates[CurrentPP + 1]->bIsActivated)
		{
			CurrentPP++;
			CurrentRail = 0;

			StartCircuit();
		}
	}
	else if (!bGoingForward)
	{
		if (PressurePlates.IsValidIndex(CurrentPP))
		{
			if (!PressurePlates[CurrentPP]->bIsActivated)
			{
				CurrentRail = CircuitPressurePlates.Find(CurrentPP)->Rails.Num() - 1;

				UAudioComponent* AudioComponent = nullptr;
				FTPP_AudioParameter Parameter = FTPP_AudioParameter();
				Parameter.Int = 17;
				SoundComponent->PlaySound(TagEletricCircuit, PressurePlates[CurrentPP]->GetActorLocation(), AudioComponent, Parameter, PressurePlates[CurrentPP]->GetRootComponent());

				StartCircuit();
			}
		}
	}
	else
	{
		// We were moving forward and there is no next plate, meaning we closed the circuit
		for (class ATPP_CircuitPressurePlate* PressurePlate : PressurePlates)
		{
			PressurePlate->OnToggled.RemoveDynamic(this, &ATPP_ElectricCircuit::HandlePressurePlateToggled);
		}

		UAudioComponent* AudioComponent = nullptr;
		FTPP_AudioParameter Parameter = FTPP_AudioParameter();
		Parameter.Int = 16;
		SoundComponent->PlaySound(TagEletricCircuit, PressurePlates[CurrentPP]->GetActorLocation(), AudioComponent, Parameter, PressurePlates[CurrentPP]->GetRootComponent());
		GEngine->AddOnScreenDebugMessage(-1,1,FColor::Green,"Finito");
		PressurePlates.Last()->TogglePlateMeshGraphics(false);
		CloseCircuit();
	}

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;
	TimerDel.BindWeakLambda
	(this,
		[this, &TimerHandle]()
		{
			EnergySphere.Get()->GetRootComponent()->SetVisibility(true);

			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		}
	);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 0.25f, false);
}
