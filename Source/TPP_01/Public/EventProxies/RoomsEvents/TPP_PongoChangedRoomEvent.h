#pragma once

#include "CoreMinimal.h"
#include "EventProxies/RoomsEvents/TPP_RoomEventBase.h"
#include "TPP_PongoChangedRoomEvent.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_PongoChangedRoomEvent : public UTPP_RoomEventBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	AActor* RoomActor;

	void Init(AActor* InRoomActor) { RoomActor = InRoomActor; }
};
