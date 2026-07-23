#pragma once

#include "CoreMinimal.h"
#include "TPP_RoomEventBase.h"
#include "TPP_ExitRoomEvent.generated.h"

/**
 * 
 */
UCLASS()
class TPP_01_API UTPP_ExitRoomEvent : public UTPP_RoomEventBase
{
	GENERATED_BODY()
public:
	UPROPERTY()
	AActor* RoomActor;
	void Init(AActor* InRoomActor) { RoomActor = InRoomActor; }
};
