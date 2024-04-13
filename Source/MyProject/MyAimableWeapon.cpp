// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyAimableWeapon.h"

#include "DrawDebugHelpers.h"
#include "MyCharacter.h"
#include "MyInGameHUD.h"

#include "Camera/CameraComponent.h"

#include "Kismet/GameplayStatics.h"

AMyAimableWeapon::AMyAimableWeapon()
{
}

bool AMyAimableWeapon::AttackImpl()
{
	if (GetWeaponStatComponent()->ConsumeAmmo())
	{
		// todo: Recoil
		UpdateAmmoDisplay();
		return true;
	}

	return false;
}

bool AMyAimableWeapon::Interact(AMyCharacter* Character)
{
	if (Super::Interact(Character))
	{
		UpdateAmmoDisplay();
		return true;
	}

	return false;
}

bool AMyAimableWeapon::ReloadImpl()
{
	if (GetWeaponStatComponent()->GetRemainingAmmoCount() > 0 && 
		 GetWeaponStatComponent()->GetCurrentAmmoCount() != GetWeaponStatComponent()->GetLoadedAmmoCount())
	{
		GetWeaponStatComponent()->Reload();
		return true;
	}

	return false;
}

void AMyAimableWeapon::OnFireRateTimed()
{
	Super::OnFireRateTimed();
}

void AMyAimableWeapon::OnReloadDone()
{
	Super::OnReloadDone();
	UpdateAmmoDisplay();
}

void AMyAimableWeapon::UpdateAmmoDisplay() const
{
	if (!IsValid(GetItemOwner()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Owner is not valid"));
		return;
	}

	if (GetItemOwner() != GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Owner is not a player, Skip"));
		return;
	}

	const auto& PlayerController = GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld());

	if (!IsValid(PlayerController))
	{
		LOG_FUNC(LogTemp, Error, "PlayerController is not valid");
		return;
	}

	const auto& HUD = Cast<AMyInGameHUD>(PlayerController->GetHUD());

	if (!IsValid(HUD))
	{
		LOG_FUNC(LogTemp, Error, "HUD is not valid");
		return;
	}

	HUD->UpdateAmmo(
				GetWeaponStatComponent()->GetCurrentAmmoCount(),
				GetWeaponStatComponent()->GetRemainingAmmoCount());

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Ammo updated"));
}

