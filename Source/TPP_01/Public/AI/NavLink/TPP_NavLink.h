#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "TPP_NavLink.generated.h"

UCLASS()
class TPP_01_API ATPP_NavLink : public ANavLinkProxy
{
	GENERATED_BODY()

public:
	ATPP_NavLink();

protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

	UFUNCTION()
	virtual void NotifyJumpLink(AActor* PathingAgent, const FVector& DestPoint);

public:
	// Z Offset to destination
	UPROPERTY(EditAnywhere, Category = "AI|Jump")
	float DestinationZOffset = 100.0f;

protected:
	UPROPERTY(Transient, meta=(MakeEditWidget="true"))
	FVector JumpDestination;
	UPROPERTY(Transient)
	UBillboardComponent* JumpDestinationBillboard;
};
