#pragma once

#include "CoreMinimal.h"
#include "TPP_DeathInfo.generated.h"

UENUM(BlueprintType)
enum class ECharacterDeath : uint8
{
	PlayerDeath,
	PongoDeath
};

UENUM(BlueprintType)
enum class EDeathCause : uint8
{
	KillBox,
	Statue,
	Pillar
};

USTRUCT(BlueprintType)
struct FDeathInfo
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bShouldSoftReset;
	UPROPERTY()
	ECharacterDeath CharacterDeath;
	UPROPERTY()
	EDeathCause DeathCause;

	UPROPERTY()
	USkeletalMeshComponent* Mesh;
};
