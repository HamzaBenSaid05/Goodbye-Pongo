#include "Components/TPP_EscapeFearRange.h"

UTPP_EscapeFearRange::UTPP_EscapeFearRange()
{
	SetGenerateOverlapEvents(false);
	SetHiddenInGame(true);
	SetCanEverAffectNavigation(false);
	PrimaryComponentTick.bCanEverTick = false;
}

void UTPP_EscapeFearRange::OnRegister()
{
	Super::OnRegister();
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetHiddenInGame(true);
}
