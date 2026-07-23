#include "EventProxies/TPP_RoomsProxySubsystem.h"

#include "DeveloperSettings/TPP_DeveloperSettings.h"

void UTPP_RoomsProxySubsystem::NotifyRoomEvent(const class UTPP_RoomEventBase* RoomEvent) { OnRoomEvent.Broadcast(RoomEvent); }

bool UTPP_RoomsProxySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
		return false;

	const UTPP_DeveloperSettings* GameplaySettings = GetDefault<UTPP_DeveloperSettings>();
	check(GameplaySettings);

	const UWorld* Level = CastChecked<UWorld>(Outer);

	return GameplaySettings->IsGameplayLevel(Level);
}
