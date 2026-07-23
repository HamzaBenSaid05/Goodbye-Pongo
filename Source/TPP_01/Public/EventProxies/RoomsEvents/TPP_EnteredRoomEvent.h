#pragma once

#include "CoreMinimal.h"
#include "TPP_RoomEventBase.h"
#include "TPP_EnteredRoomEvent.generated.h"

UCLASS()
class TPP_01_API UTPP_EnteredRoomEvent : public UTPP_RoomEventBase
{
	GENERATED_BODY()
public:
	UPROPERTY()
	AActor* RoomActor;
	void Init(AActor* InRoomActor) { RoomActor = InRoomActor; }
};
