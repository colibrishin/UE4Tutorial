// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyPlayerController.h"

#include "Private/CommonBuy.hpp"
#include "MyCameraManager.h"
#include "MyCharacter.h"
#include "MyInGameHUD.h"
#include "MyInGameWidget.h"
#include "MyPlayerState.h"
#include "MySpectatorPawn.h"
#include "MyWeapon.h"
#include "MyWeaponDataAsset.h"

#include "GameFramework/GameStateBase.h"
#include "Interfaces/MyPlayerStateRequiredWidget.h"

AMyPlayerController::AMyPlayerController()
{
	PlayerCameraManagerClass = AMyCameraManager::StaticClass();
}

void AMyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AMyPlayerState* MyPlayerState = Cast<AMyPlayerState>(PlayerState);

	if (!IsValid(MyPlayerState))
	{
		return;
	}

	// Player state replication would be happened in client side and player controller exists in local player. 
	if (const AMyInGameHUD* HUD = Cast<AMyInGameHUD>(GetHUD()))
	{
		// Iterate all widgets and pass the own player state to the widgets that require the player state. 
		for (TFieldIterator<FObjectProperty> Iterator(UMyInGameWidget::StaticClass());
			Iterator;
			++Iterator)
		{
			UUserWidget* Widget = Cast<UUserWidget>(Iterator->GetObjectPropertyValue(Iterator->ContainerPtrToValuePtr<void>(HUD->GetInGameWidget(), 0)));
			if (IMyPlayerStateRequiredWidget* Interface = Cast<IMyPlayerStateRequiredWidget>(Widget))
			{
				Interface->DispatchPlayerState(MyPlayerState);
			}
		} 
	}
}

void AMyPlayerController::ProcessBuy(AMyCharacter* RequestCharacter, const int32 WeaponID) const
{
	if (HasAuthority())
	{
		const auto& WeaponData        = GetRowData<FMyWeaponData>(this, WeaponID);
		const auto& WeaponClass       = WeaponData->WeaponDataAsset->GetWeaponClass();
		const auto& CharacterLocation = RequestCharacter->GetActorLocation();

		GetPlayerState<AMyPlayerState>()->AddMoney
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
			GeneratedWeapon->SetOwner(RequestCharacter);
			Cast<AMyWeapon>(GeneratedWeapon)->Server_Interact(RequestCharacter);
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

void AMyPlayerController::SetSpectator(AMySpectatorPawn* Spectator)
{
	UnPossess();
	Possess(Spectator);
}


void AMyPlayerController::Client_SetSpectator_Implementation(AMySpectatorPawn* Spectator)
{
	SetSpectator(Spectator);
}