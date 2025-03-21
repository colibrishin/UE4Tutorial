#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"

UENUM()
enum class EMultiShapeType : uint8
{
	Box,
	Sphere,
	Capsule
};

UENUM()
enum class EMyWeaponType : uint8
{
	Unknown,
	Range,
	Melee,
	Throwable
};

UENUM()
enum class EMySlotType : uint8
{
	Unknown,
	Primary,
	Secondary,
	Melee,
	Utility,
	Bomb
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
	Idle,
	Planting,
	Planted,
	Defusing,
	Defused,
	Exploded
};