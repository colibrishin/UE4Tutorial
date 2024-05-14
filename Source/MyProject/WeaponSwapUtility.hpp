#pragma once

#include "MyCharacter.h"
#include "MyInventoryComponent.h"
#include "MyPlayerState.h"

__forceinline void CharacterSwapHand(const AMyCharacter* Character, const int Index)
{
	const auto& Inventory = Character->GetInventory();

	if (const auto& Collectable = Inventory->Get(Index))
	{
		const auto& PlayerState = Character->GetPlayerState<AMyPlayerState>();

		if (Collectable == PlayerState->GetCurrentHand())
		{
			return;
		}

		if (IsValid(Collectable))
		{
			PlayerState->SetCurrentItem(Collectable);
		}
	}
}