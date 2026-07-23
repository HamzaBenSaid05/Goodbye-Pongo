#pragma once

// Enum used for VFX purposes, 
// the order and values of each entry reflect the texture used for SubUVAnimation,
// change only if aware of how the texture used is composed
UENUM()
enum class ETPP_DrumCommands : int32
{
	None = INDEX_NONE,
	Stop = 0,
	Interact = 1,
	GoThere = 2,
	ComeHere = 3,
};
