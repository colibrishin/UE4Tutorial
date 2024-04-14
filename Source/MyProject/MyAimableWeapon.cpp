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

bool AMyAimableWeapon::PreInteract(AMyCharacter* Character)
{
	return Super::PreInteract(Character);
}

bool AMyAimableWeapon::PostInteract(AMyCharacter* Character)
{
	const auto& Result = Super::PostInteract(Character);

	if (Result)
	{
		UpdateAmmoDisplay();
	}
	
	return Result;
}

bool AMyAimableWeapon::TryAttachItem(const AMyCharacter* Character)
{
	LOG_FUNC(LogTemp, Warning, "TryAttachItem");

	if (GetMesh()->AttachToComponent
		(
		 Character->GetMesh(),
		 FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		 AMyCharacter::RightHandSocketName
		))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("AttachToComponent success"));
		return true;
	}
	else
	{
		const FVector PreviousLocation = GetActorLocation();
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("AttachToComponent failed"));
		SetActorLocation(PreviousLocation);
		return false;
	}
}

bool AMyAimableWeapon::PreUse(AMyCharacter* Character)
{
	return Super::PreUse(Character);
}

bool AMyAimableWeapon::PostUse(AMyCharacter* Character)
{
	return Super::PostUse(Character);
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

