#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TPP_RoomsProxySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRoomEventDelegate, const class UTPP_RoomEventBase*, RoomEvent);

UCLASS()
class TPP_01_API UTPP_RoomsProxySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FRoomEventDelegate OnRoomEvent;

	UFUNCTION(BlueprintCallable)
	void NotifyRoomEvent(const class UTPP_RoomEventBase* RoomEvent);

private:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
};
