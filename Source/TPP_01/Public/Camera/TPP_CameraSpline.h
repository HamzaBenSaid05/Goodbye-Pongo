#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPP_CameraSpline.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerEnteredRoomDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerExitRoomDelegate);

UCLASS()
class TPP_01_API ATPP_CameraSpline : public AActor
{
	GENERATED_BODY()

public:
	ATPP_CameraSpline();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USplineComponent* SplineComponent;

	UPROPERTY(Instanced, VisibleAnywhere, BlueprintReadOnly, Category = "Spline Data")
	class UTPP_CameraSplineMetadata* SplineMetadata;

	UPROPERTY(BlueprintAssignable)
	FPlayerEnteredRoomDelegate OnPlayerEnteredRoom;
	UPROPERTY(BlueprintAssignable)
	FPlayerExitRoomDelegate OnPlayerExitRoom;

	virtual void PostLoad() override;
	virtual void PostRegisterAllComponents() override;
	void FillArrayWithSplinePosition(TArray<FVector>& InSplinePosition) const;

	void GetInterpolatedCameraData(
		const float& Distance, float& OutBias, FRotator& OutRotation, float& OutSpringArmLength,
		float& OutYOffset, float& OutZOffset, float& OutBiasInterpSpeed,
		float& OutLocationInterpSpeed, float& OutRotationInterpSpeed, AActor*& OutActor) const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	TArray<FVector> PassRoomEdgePointsToCamera() const;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void PlayerEnteredExitRoom(const class UTPP_RoomEventBase* RoomEvent);
};
