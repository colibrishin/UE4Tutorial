// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyPlayerController.h"

#include "CommonBuy.hpp"
#include "Data.h"
#include "MyCameraManager.h"
#include "MyCharacter.h"
#include "MyInGameHUD.h"
#include "MyPlayerState.h"
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
	if (!ValidateBuyRequest(WeaponID, RequestCharacter))
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

	if (!ValidateBuyRequest(WeaponID, RequestCharacter))
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
