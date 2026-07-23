#include "Components/TPP_NavMeshHandler.h"

#include "Components/BrushComponent.h"

void UTPP_NavMeshHandler::OnRegister()
{
	Super::OnRegister();
	UpdateNavModifierVolume();
}

#if WITH_EDITOR
void UTPP_NavMeshHandler::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateNavModifierVolume();
}
#endif

void UTPP_NavMeshHandler::UpdateNavModifierVolume() const
{
	if (!IsValid(NavModifierVolume)) { return; }
	if (UBrushComponent* Brush = NavModifierVolume->GetBrushComponent()) { Brush->SetMobility(EComponentMobility::Movable); }
	if (bAttachToOwner)
	{
		AActor* Owner = GetOwner();
		if (!IsValid(Owner)) { return; }

		if (NavModifierVolume->GetAttachParentActor() != Owner)
		{
			NavModifierVolume->AttachToActor(
											 Owner,
											 FAttachmentTransformRules::KeepWorldTransform
											);
		}
	}
	else
	{
		if (NavModifierVolume->GetAttachParentActor() != nullptr)
		{
			NavModifierVolume->DetachFromActor(
											   FDetachmentTransformRules::KeepWorldTransform
											  );
		}
	}
}