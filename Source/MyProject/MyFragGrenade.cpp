// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyFragGrenade.h"

#include "MyCharacter.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "MyStatComponent.h"

AMyFragGrenade::AMyFragGrenade() : IsThrown(false), IsExploded(false)
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Grenade(TEXT("SkeletalMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/G67_Grenade/SK_G67_X.SK_G67_X'"));

	SetSkeletalMesh();

	if (SK_Grenade.Succeeded())
	{
		GetSkeletalMeshComponent()->SetSkeletalMesh(SK_Grenade.Object);
	}
}

bool AMyFragGrenade::AttackImpl()
{
	Charge();
	return true;
}

bool AMyFragGrenade::AttackInterruptedImpl()
{
	const auto& Result = Super::AttackInterruptedImpl();

	if (Result)
	{
		Throw();
	}

	return Result;
}

bool AMyFragGrenade::ReloadImpl()
{
	return false;
}

void AMyFragGrenade::DropLocation()
{
	if (IsThrown)
	{
		// Override base DropLocation
		const auto& ForwardPosition = GetActorLocation() + PreviousOwner->GetActorForwardVector() * 100.f;
		SetActorLocation(ForwardPosition);
	}
	else
	{
		Super::DropLocation();
	}
}

void AMyFragGrenade::Throw()
{
	ExecuteServer(this, 
				   &AMyFragGrenade::Multi_Throw,
				   &AMyFragGrenade::ThrowImpl);
}

void AMyFragGrenade::Charge()
{
	ExecuteServer
		(
		 this,
		 &AMyFragGrenade::Multi_Charge,
		 &AMyFragGrenade::ChargeImpl
		);
}

void AMyFragGrenade::Multi_Charge_Implementation()
{
	ChargeImpl();
}

void AMyFragGrenade::ChargeImpl()
{
	GetWorldTimerManager().SetTimer
	(
		OnExplosionTimerExpiredHandle,
		this,
		&AMyFragGrenade::OnExplosionTimerExpired,
		3.f,
		false
	);
}

void AMyFragGrenade::Multi_Throw_Implementation()
{
	ThrowImpl();
}

void AMyFragGrenade::ThrowImpl()
{
	if (IsExploded || IsThrown)
	{
		return;
	}

	IsThrown = true;

	PreviousOwner = GetItemOwner();

	Drop();
	GetSkeletalMeshComponent()->AddImpulse(FVector::ForwardVector * 1000.f, NAME_None, true);
}

void AMyFragGrenade::OnExplosionTimerExpired()
{
	IsExploded = true;

	// todo: explosion effect

	if (HasAuthority())
	{
		TArray<FOverlapResult> HitResults;

		const auto& Radius = GetWeaponStatComponent()->GetRadius();
		const auto& Damage = GetWeaponStatComponent()->GetDamage();

		GetWorld()->OverlapMultiByChannel
		(
			OUT HitResults,
			GetActorLocation(),
			FQuat::Identity,
			ECollisionChannel::ECC_Pawn,
			FCollisionShape::MakeSphere(Radius)
		);

		if (!PreviousOwner.IsValid())
		{
			LOG_FUNC(LogTemp, Error, "Previous owner is not noted");
			return;
		}

		for (const auto& Result : HitResults)
		{
			if (const auto& Character = Cast<AMyCharacter>(Result.GetActor()))
			{
				const auto& Distance = FVector::Distance(Character->GetActorLocation(), GetActorLocation());
				const auto& Ratio = 1.f - Distance / Radius;
				const auto& RatioDamage = Damage * Ratio;

				Character->GetPlayerState<AMyPlayerState>()->TakeDamage
				(
					RatioDamage, 
					{}, 
					Cast<AMyPlayerController>(PreviousOwner->GetOwner()), 
					PreviousOwner.Get()
				);
			}
		}
	}

	Destroy(true);
}
