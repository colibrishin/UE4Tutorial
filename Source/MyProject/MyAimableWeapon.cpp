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

bool AMyAimableWeapon::Attack()
{
	if (GetWeaponStatComponent()->ConsumeAmmo())
	{
		Super::Attack();

		GetWorld()->GetTimerManager().SetTimer(
			FireRateTimerHandle, 
			this, 
			&AMyAimableWeapon::ResetFire, 
			GetWeaponStatComponent()->GetFireRate(), 
			false);

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

void AMyAimableWeapon::Reload()
{
	Super::Reload();

	GetWorld()->GetTimerManager().SetTimer
		(
		 ReloadTimerHandle,
		 this,
		 &AMyAimableWeapon::ReloadImpl,
		 GetWeaponStatComponent()->GetReloadTime(),
		 false
		);
}

void AMyAimableWeapon::ResetFire()
{
	GetWorld()->GetTimerManager().ClearTimer(FireRateTimerHandle);
	OnFireReady.Broadcast();
}

void AMyAimableWeapon::ReloadImpl()
{
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	GetWeaponStatComponent()->Reload();

	Cast<AMyInGameHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD())->UpdateAmmo(
		 GetWeaponStatComponent()->GetCurrentAmmoCount(),
		 GetWeaponStatComponent()->GetRemainingAmmoCount());

	OnFireReady.Broadcast();
}
