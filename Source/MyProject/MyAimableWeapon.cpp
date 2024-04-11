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
		Cast<AMyInGameHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD())->UpdateAmmo(
			 GetWeaponStatComponent()->GetCurrentAmmoCount(),
			 GetWeaponStatComponent()->GetRemainingAmmoCount());

		return true;
	}

	return false;
}

bool AMyAimableWeapon::Interact(AMyCharacter* Character)
{
	if (Super::Interact(Character))
	{
		Cast<AMyInGameHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD())->UpdateAmmo(
			 GetWeaponStatComponent()->GetCurrentAmmoCount(),
			 GetWeaponStatComponent()->GetRemainingAmmoCount());

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

	Cast<AMyInGameHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD())->UpdateAmmo(
		 GetWeaponStatComponent()->GetCurrentAmmoCount(),
		 GetWeaponStatComponent()->GetRemainingAmmoCount());
}

