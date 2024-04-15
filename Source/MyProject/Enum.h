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
	Idle,
	Running,
	Attacking,
	Planting,
	Defusing,
	Dead
};