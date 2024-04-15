#pragma once

#include "CoreMinimal.h"
#include "Data.h"
#include "MyCharacter.h"
#include "MyGameState.h"
#include "MyPlayerState.h"
#include "MyStatComponent.h"
#include "MyWeaponDataAsset.h"
#include "Utilities.hpp"

inline bool ValidateBuyRequest(const int32 ID, AMyCharacter* const& Character)
{
	const auto& Instance   = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(Character));
	const auto& WeaponData = GetWeaponData(Character, ID);
	const auto& WeaponStat = WeaponData->WeaponDataAsset->GetWeaponStat();

	if (WeaponData == nullptr)
	{
		LOG_FUNC(LogTemp, Error, "WeaponInfo is nullptr");
		return false;
	}

	if (Character->GetStatComponent()->GetMoney() <= 0)
	{
		LOG_FUNC(LogTemp, Error, "Player => Not enough money");
		return false;
	}

	if (Character->GetStatComponent()->GetMoney() - WeaponStat.Price < 0)
	{
		LOG_FUNC(LogTemp, Error, "Player has money but have not enough money to buy");
		return false;
	}

	const auto& PlayerState = Cast<AMyGameState>(UGameplayStatics::GetGameState(Character));

	if (!PlayerState->CanBuy())
	{
		LOG_FUNC(LogTemp, Error, "MatchBuyTime is over");
		return false;
	}

	return true;
}
