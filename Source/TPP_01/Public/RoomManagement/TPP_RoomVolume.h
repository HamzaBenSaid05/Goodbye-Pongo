#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPP_RoomVolume.generated.h"

UCLASS()
class TPP_01_API ATPP_RoomVolume : public AActor
{
	GENERATED_BODY()

public:
	ATPP_RoomVolume();

	UFUNCTION(blueprintPure)
	FORCEINLINE class UBoxComponent* GetVolume() const { return Volume; }
	
	UFUNCTION(blueprintPure)
	const TArray<FVector>& GetEdgePoints() const { return EdgePoints; }

protected:
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Volume;
	
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget=true))
	TArray<FVector> EdgePoints;

	UFUNCTION()
	void PlayerEnteredRoom(UPrimitiveComponent* OverlappedComponent,
	                       AActor* OtherActor,
	                       UPrimitiveComponent* OtherComp,
	                       int32 OtherBodyIndex,
	                       bool bFromSweep,
	                       const FHitResult& SweepResult);

	UFUNCTION()
	void PlayerExitRoom(UPrimitiveComponent* OverlappedComponent,
	                    AActor* OtherActor,
	                    UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex);
};
