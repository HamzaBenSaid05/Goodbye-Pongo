#include "VFX/PongoInteractables/TPP_InteractionEffectAnchor.h"

#include "NiagaraComponent.h"

ATPP_InteractionEffectAnchor::ATPP_InteractionEffectAnchor()
{
	PlayingVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PlayingVFX"));
	SetRootComponent(PlayingVFX);
	
	TrailVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailVFX"));
	TrailVFX->SetupAttachment(GetRootComponent());
}

void ATPP_InteractionEffectAnchor::PlayVFX(float InSize)
{
	PlayingVFX->SetVariableFloat("StartingSize", InSize);
	PlayingVFX->Activate(true);
}

void ATPP_InteractionEffectAnchor::ToggleTrail(const bool bActivate) 
{
	TrailVFX->SetActive(bActivate);
}
