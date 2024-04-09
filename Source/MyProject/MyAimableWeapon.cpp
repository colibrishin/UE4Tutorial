// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyAimableWeapon.h"

#include "DrawDebugHelpers.h"
#include "MyCharacter.h"

#include "Camera/CameraComponent.h"

AMyAimableWeapon::AMyAimableWeapon()
	: bCanFire(true)
{
}

void AMyAimableWeapon::Fire()
{
	const auto& Character = GetItemOwner();

	if (!IsValid(Character))
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is not valid"));
		return;
	}

	if (!bCanFire)
	{
		return;
	}

	const auto& Camera = Cast<UCameraComponent>(Character->GetComponentByClass(UCameraComponent::StaticClass()));

	if (!IsValid(Camera))
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Fire!"));

	FHitResult HitResult;
	FCollisionQueryParams Params{NAME_None, false, Character};

	FVector EndVector = Camera->GetComponentLocation() + Camera->GetForwardVector() * Range;

	const auto& Result = GetWorld()->LineTraceSingleByChannel
	(
		OUT HitResult,
		Camera->GetComponentLocation(),
		EndVector,
		ECollisionChannel::ECC_Visibility,
		Params
	);

	if (Result)
	{
		DrawDebugLine(
			GetWorld(),
			Camera->GetComponentLocation(),
			HitResult.ImpactPoint,
			FColor::Green,
			false,
			1.f,
			0,
			2.f
		);

		const auto& Target = Cast<AMyCharacter>(HitResult.GetActor());

		if (IsValid(Target))
		{
			// todo: refactoring
			Target->TakeDamage
			(
				GetDamage(),
				FDamageEvent{},
				GetInstigatorController(),
				Character
			);
		}
	}
	else
	{
		DrawDebugLine(
			GetWorld(),
			Camera->GetComponentLocation(),
			EndVector,
			FColor::Red,
			false,
			1.f,
			0,
			2.f
		);
	}

	GetWorld()->GetTimerManager().SetTimer(OUT FireRateTimerHandle, IN this, &AMyAimableWeapon::ResetFire, FireRate);
}

void AMyAimableWeapon::ResetFire()
{
	bCanFire = true;
	GetWorld()->GetTimerManager().ClearTimer(FireRateTimerHandle);
	OnFireReady.Broadcast();
}
