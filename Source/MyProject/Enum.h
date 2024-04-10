#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"

UENUM()
enum EMyWeaponType
{
	Unknown,
	Range,
	Melee
};

UENUM()
enum EMyCharacterState
{
	Idle,
	Running,
	Attacking,
	Planting,
	Defusing,
	Dead
};