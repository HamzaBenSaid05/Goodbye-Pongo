#include "VFX/PongoInteractables/PulseHandler.h"

#include "VFX/PongoInteractables/TPP_InteractableHighlightingSubsystem.h"

UPulseHandler::UPulseHandler()
{
	bWantsInitializeComponent = true;
	bAutoActivate = true;
}

void UPulseHandler::TogglePulse()
{
	if (!IsValid(ItemMesh)) return;

	ItemMesh->SetRenderCustomDepth(!ItemMesh->bRenderCustomDepth);
}

void UPulseHandler::InitializeComponent()
{
	Super::InitializeComponent();

	ItemMesh = GetOwner()->FindComponentByClass<UMeshComponent>();
	ItemMesh->SetCustomDepthStencilValue(1);
}

void UPulseHandler::BeginPlay()
{
	Super::BeginPlay();

	if (!IsActive()) return;
	
	if (UTPP_InteractableHighlightingSubsystem* HighlightSubsystem = GetWorld()->GetSubsystem<UTPP_InteractableHighlightingSubsystem>())
	{
		HighlightSubsystem->RegisterActor(this);
	}
}

void UPulseHandler::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (!IsActive()) return;
	
	if (UTPP_InteractableHighlightingSubsystem* HighlightSubsystem = GetWorld()->GetSubsystem<UTPP_InteractableHighlightingSubsystem>())
	{
		HighlightSubsystem->UnregisterActor(this);
	}
}
