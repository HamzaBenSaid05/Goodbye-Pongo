#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PulseHandler.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPP_01_API UPulseHandler : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UPulseHandler();
	void TogglePulse();

protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	UPROPERTY()
	TObjectPtr<UMeshComponent> ItemMesh;
};
