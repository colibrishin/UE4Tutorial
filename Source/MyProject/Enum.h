#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"

UENUM()
enum class EMyWeaponType : uint8
{
	Unknown,
	Range,
	Melee
};

UENUM()
enum class EMyCharacterState : uint8
{
	Unknown,
	Alive,
	Planting,
	Defusing,
	Dead
};

UENUM()
enum class EMyTeam : uint8
{
	Unknown,
	CT,
	T
};

UENUM()
enum class EMyRoundProgress : uint8
{
	Unknown,
	FreezeTime,
	Playing,
	PostRound,
	Max
};

UENUM()
enum class EMyBombState : uint8
{
	Unknown,
	Idle,
	Planting,
	Planted,
	Defusing,
	Defused,
	Exploded
};