#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPP_Camera.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPlayerCamera, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCameraChangeDelegate, AActor*, NewCamera, float, BlendTime);

UCLASS()
class TPP_01_API ATPP_Camera : public AActor
{
	GENERATED_BODY()

public:
	ATPP_Camera();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComp; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class USpringArmComponent* GetSpringArmComponent() const { return SpringArmComp; }

	UPROPERTY(BlueprintReadWrite)
	AActor* CurrentObjectRef;

	UPROPERTY(BlueprintAssignable)
	FCameraChangeDelegate OnCameraChange;

	UFUNCTION(BlueprintPure)
	FORCEINLINE AActor* GetPlayerRef() const { return PlayerRef; }

	UFUNCTION(BlueprintCallable)
	void SetPlayerRef(AActor* Character);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCompanionRef(AActor* Character) { CompanionRef = Character; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetBlendTime() const { return BlendTime; }

	UPROPERTY(BlueprintReadOnly)
	bool bIsInUse;

	// Used only in feature testing levels
	void CalculateEdgePointsFromSpline();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void UpdateCameraPosition(const float& DeltaSeconds);

	FVector FindCameraBias(const float& DeltaSeconds);

	UPROPERTY(BlueprintReadOnly)
	AActor* PlayerRef;

	UPROPERTY(BlueprintReadOnly)
	AActor* CompanionRef;

private:
	UPROPERTY(EditAnywhere, Category="Room")
	float BlendTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	class UTPP_CameraZoomComponent* CameraZoomComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Position", meta = (AllowPrivateAccess = true))
	float MaxDistanceFromPlayer = 500.f;

	UPROPERTY(EditAnywhere, Category="Camera Position")
	float LocationInterpSpeed;

	UPROPERTY(EditAnywhere, Category="Camera Position")
	float RotationInterpSpeed;

	UPROPERTY(EditAnywhere, Category="Camera Position")
	float BiasInterpSpeed;

	UPROPERTY(EditAnywhere, Category = "Camera Position",
		meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0", Delta="0.1"))
	float CameraBiasPlayerToObject = 0.5f;

	UPROPERTY()
	int32 Iterations = 10;

	UPROPERTY()
	float TargetBiasFromSpline;

	UPROPERTY()
	float FinalCameraSpringArm;

	UPROPERTY()
	float FinalCameraYOffset;

	UPROPERTY()
	float FinalCameraXOffset;

	UPROPERTY()
	float FinalCameraZOffset;

	UPROPERTY()
	float CurrentAlpha;

	UPROPERTY()
	FVector TargetPoint;

	UPROPERTY()
	FVector SmoothedObjectLocation;

	UPROPERTY()
	TArray<FVector> EdgePoints;

	UPROPERTY()
	FRotator FinalSplineRotation;

	UPROPERTY()
	TArray<FVector> SplinePosition;

	UPROPERTY()
	class USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class ATPP_CameraSpline> CameraSpline;

	UFUNCTION()
	void PlayerEnteredThisRoom();

	UFUNCTION()
	void PlayerExitThisRoom();

	UFUNCTION()
	void GetEdgePointsFromSpline(const TArray<FVector>& InEdgePoints);
};
