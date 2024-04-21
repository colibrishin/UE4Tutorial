#pragma once

#include "CoreMinimal.h"
#include "Data.h"
#include "MyBuyZone.h"
#include "MyCharacter.h"
#include "MyGameState.h"
#include "MyPlayerState.h"
#include "MyStatComponent.h"
#include "MyWeaponDataAsset.h"
#include "Utilities.hpp"

inline bool IsPlayerInBuyZone(AMyCharacter* Character)
{
	const FCollisionQueryParams Params {NAME_None, false, Character};

	const auto& PlayerState = Character->GetPlayerState<AMyPlayerState>();

	TArray<FOverlapResult> Results;

	const auto& BuyCheck = Character->GetWorld()->OverlapMultiByChannel
	(
		Results,
		Character->GetActorLocation(),
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel8,
		FCollisionShape::MakeSphere(10.0f),
		Params
	);

	bool Found = false;

	for (const auto& Result : Results)
	{
		const auto& BuyZone = Cast<AMyBuyZone>(Result.GetActor());

		if (!IsValid(BuyZone))
		{
			continue;
		}

		LOG_FUNC_PRINTF(LogTemp, Warning, "BuyZone: %s", *BuyZone->GetName());

		if (BuyZone->GetTeam() == PlayerState->GetTeam())
		{
			Found = true;
		}
	}

	if (!Found)
	{
		LOG_FUNC(LogTemp, Error, "Player is not in buy zone");
		return false;
	}

	return true;
}

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

	if (Character->GetPlayerState<AMyPlayerState>()->GetMoney() <= 0)
	{
		LOG_FUNC(LogTemp, Error, "Player => Not enough money");
		return false;
	}

	if (Character->GetPlayerState<AMyPlayerState>()->GetMoney() - WeaponStat.Price < 0)
	{
		LOG_FUNC(LogTemp, Error, "Player has money but have not enough money to buy");
		return false;
	}

	const auto& GameState = Cast<AMyGameState>(UGameplayStatics::GetGameState(Character));

	if (!GameState->CanBuy())
	{
		LOG_FUNC(LogTemp, Error, "MatchBuyTime is over");
		return false;
	}

	return IsPlayerInBuyZone(Character);
}

