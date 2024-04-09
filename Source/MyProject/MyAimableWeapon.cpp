// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyAimableWeapon.h"

#include "DrawDebugHelpers.h"
#include "MyCharacter.h"

#include "Camera/CameraComponent.h"

AMyAimableWeapon::AMyAimableWeapon()
{
}

void AMyAimableWeapon::Attack()
{
	Super::Attack();
	GetWorld()->GetTimerManager().SetTimer(
		FireRateTimerHandle, 
		this, 
		&AMyAimableWeapon::ResetFire, 
		GetWeaponStatComponent()->GetFireRate(), 
		false);
}

void AMyAimableWeapon::ResetFire()
{
	GetWorld()->GetTimerManager().ClearTimer(FireRateTimerHandle);
	OnFireReady.Broadcast();
}
