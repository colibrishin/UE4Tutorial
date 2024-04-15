// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyPlayerController.h"

#include "Data.h"
#include "MyCameraManager.h"
#include "MyCharacter.h"
#include "MyInGameHUD.h"
#include "MyStatComponent.h"
#include "MyWeapon.h"
#include "MyWeaponDataAsset.h"
#include "Utilities.hpp"

#include "GameFramework/GameStateBase.h"

AMyPlayerController::AMyPlayerController()
{
	PlayerCameraManagerClass = AMyCameraManager::StaticClass();
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AMyPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

bool AMyPlayerController::BuyWeapon_Validate(AMyCharacter* RequestCharacter , const int32 WeaponID) const
{
	if (!IsValid(RequestCharacter))
	{
		LOG_FUNC(LogTemp, Error, "Player is invalid");
		return false;
	}

	const auto& WeaponData = GetWeaponData(this, WeaponID);
	const auto& WeaponStat = WeaponData->WeaponDataAsset->GetWeaponStat();

	// todo: discard buy if player has same weapon.

	if (RequestCharacter->GetWeapon()->GetClass() == WeaponData->WeaponDataAsset->GetWeaponClass())
	{
		LOG_FUNC(LogTemp, Error, "Player already has the weapon");
		return false;
	}

	if (WeaponData == nullptr)
	{
		LOG_FUNC(LogTemp, Error, "WeaponInfo is nullptr");
		return false;
	}

	if (RequestCharacter->GetStatComponent()->GetMoney() <= 0)
	{
		LOG_FUNC(LogTemp, Error, "Player => Not enough money");
		return false;
	}

	if (RequestCharacter->GetStatComponent()->GetMoney() - WeaponStat.Price < 0)
	{
		LOG_FUNC(LogTemp, Error, "Player has money but not enough money to buy");
		return false;
	}

	if (const auto& BuyTime = Cast<AMyProjectGameModeBase>(UGameplayStatics::GetGameMode(this))->HasBuyTimeEnded())
	{
		LOG_FUNC(LogTemp, Error, "MatchBuyTime is over");
		return false;
	}

	return true;
}

void AMyPlayerController::ProcessBuy(AMyCharacter* RequestCharacter, const int32 WeaponID) const
{
	if (HasAuthority())
	{
		const auto& WeaponData        = GetWeaponData(this, WeaponID);
		const auto& WeaponClass       = WeaponData->WeaponDataAsset->GetWeaponClass();
		const auto& CharacterLocation = RequestCharacter->GetActorLocation();

		RequestCharacter->GetStatComponent()->AddMoney
			(
			 -WeaponData->WeaponDataAsset->GetWeaponStat().Price
			);

		const auto& GeneratedWeapon = GetWorld()->SpawnActor
			(
			 WeaponClass,
			 &CharacterLocation,
			 nullptr
			);

		LOG_FUNC_PRINTF(LogTemp, Warning, "Buying Weapon: %s", *WeaponData->WeaponDataAsset->GetWeaponStat().Name);

		if (IsValid(GeneratedWeapon))
		{
			GeneratedWeapon->SetReplicateMovement(true);
			GeneratedWeapon->SetReplicates(true);
			Cast<AMyWeapon>(GeneratedWeapon)->Interact(RequestCharacter);
		}
	}
}

void AMyPlayerController::Server_BuyWeapon_Implementation(AMyCharacter* RequestCharacter, const int32 WeaponID) const
{
	if (!BuyWeapon_Validate(RequestCharacter, WeaponID))
	{
		LOG_FUNC(LogTemp, Error, "BuyWeapon_Validate failed");
		return;
	}

	if (!IsValid(RequestCharacter))
	{
		LOG_FUNC(LogTemp, Error, "Player is invalid");
		return;
	}

	ProcessBuy(RequestCharacter, WeaponID);
}

void AMyPlayerController::BuyWeapon(const int32 WeaponID) const
{
	const auto& RequestCharacter = Cast<AMyCharacter>(GetPawn());

	if (!IsValid(RequestCharacter))
	{
		LOG_FUNC(LogTemp, Error, "Player is invalid");
		return;
	}

	if (!BuyWeapon_Validate(RequestCharacter, WeaponID))
	{
		LOG_FUNC(LogTemp, Error, "BuyWeapon_Validate failed");
		return;
	}

	if (IsValid(RequestCharacter))
	{
		ExecuteServer(
			this, 
			&AMyPlayerController::Server_BuyWeapon, 
			&AMyPlayerController::ProcessBuy,
			RequestCharacter,
			WeaponID);
	}

}
