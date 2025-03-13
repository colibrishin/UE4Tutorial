#pragma once

#include "CoreMinimal.h"
#include "Data.h"
#include "MyProject/MyGameState.h"
#include "MyProject/MyPlayerState.h"
#include "Utilities.hpp"

#include "Engine/OverlapResult.h"

#include "Kismet/GameplayStatics.h"

#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Actors/MyBuyZone.h"
#include "MyProject/DataAsset/DA_Weapon.h"
#include "MyProject/Frameworks/Subsystems/SS_World.h"

class USS_World;

inline bool IsPlayerInBuyZone(AA_Character* Character)
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

inline bool ValidateBuyRequest(const int32 ID, AA_Character* const& Character)
{
	const auto& WeaponData = Character->GetWorld()->GetSubsystem<USS_World>()->GetRowData<FBaseAssetRow>(ID);
	const UDA_Weapon* Downcast = Cast<UDA_Weapon>(WeaponData->AssetToLink);

	if (!Downcast)
	{
		LOG_FUNC(LogTemp, Error, "Invalid collectable information, possibly not a weapon");
	}
	
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

	if (Character->GetPlayerState<AMyPlayerState>()->GetMoney() - Downcast->GetPrice() < 0)
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

	if ( Character->GetHand()->GetChildActor() )
	{
		LOG_FUNC( LogTemp , Error , "Character has the weapon" );
		return false;
	}

	return IsPlayerInBuyZone(Character);
}

