#pragma once

UENUM(BlueprintType)
enum class ETPP_PongoState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Stroll UMETA(DisplayName = "Stroll"),
	FearPlayer UMETA(DisplayName = "FearPlayer"),
	Fear UMETA(DisplayName = "Fear"),
	Curiosity UMETA(DisplayName = "Curiosity"),
	Rage UMETA(DisplayName = "Rage"),
	Trust UMETA(DisplayName = "Trust"),
	Scripted UMETA(DisplayName = "Scripted"),
};

UENUM()
enum class ETPP_PongoReactions : uint8
{
	None,
	Fear,
	Curiosity,
	Reassured,
	Unconcerned,
	Annoyed,
	Strolling,
};
