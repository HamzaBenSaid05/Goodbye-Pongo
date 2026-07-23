#include "Item/Interactable/TPP_OrbItem.h"

#include "Components/TPP_SoundComponent.h"
#include "EventProxies/TPP_AudioProxySubsystem.h"

void ATPP_OrbItem::BeginPlay()
{
	Super::BeginPlay();
	FTPP_AudioParameter Parameter = FTPP_AudioParameter();
	Parameter.Int = OrbParameter;
	SoundComponent->PlaySound(TagOrb,GetActorLocation(),AudioComOrb,Parameter,GetRootComponent());
}

