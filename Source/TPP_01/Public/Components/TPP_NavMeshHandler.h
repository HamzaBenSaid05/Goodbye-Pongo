#pragma once

#include "CoreMinimal.h"
#include "NavModifierVolume.h"
#include "Components/ActorComponent.h"
#include "TPP_NavMeshHandler.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPP_01_API UTPP_NavMeshHandler : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NavMesh")
	TObjectPtr<ANavModifierVolume> NavModifierVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NavMesh")
	bool bAttachToOwner = true;

protected:
	virtual void OnRegister() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	void UpdateNavModifierVolume() const;
};