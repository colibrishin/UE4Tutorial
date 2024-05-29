// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyGrenade.h"

#include "MyCharacter.h"

bool AMyGrenade::AttackImpl()
{
	Charge();
	return true;
}

bool AMyGrenade::AttackInterruptedImpl()
{ 
	const auto& Result = Super::AttackInterruptedImpl();

	if (Result)
	{
		Throw();
	}

	return Result;
}

bool AMyGrenade::ReloadImpl()
{
	return false;
}

void AMyGrenade::DropLocation()
{
	if (IsThrown)
	{
		// Override base DropLocation
		const auto& ForwardPosition = PreviousOwner->GetActorLocation() + PreviousOwner->GetActorForwardVector() * 100.f;
		SetActorLocation(ForwardPosition);
	}
	else
	{
		Super::DropLocation();
	}
}

void AMyGrenade::OnExplosionTimerExpiredImpl()
{
	
}

void AMyGrenade::Throw()
{
	ExecuteServer(this, 
				   &AMyGrenade::Multi_Throw,
				   &AMyGrenade::ThrowImpl);
}

void AMyGrenade::Charge()
{
	ExecuteServer
		(
		 this,
		 &AMyGrenade::Multi_Charge,
		 &AMyGrenade::ChargeImpl
		);
}

void AMyGrenade::Multi_Charge_Implementation()
{
	ChargeImpl();
}

void AMyGrenade::ChargeImpl()
{
	GetWorldTimerManager().SetTimer
	(
		OnExplosionTimerExpiredHandle,
		this,
		&AMyGrenade::OnExplosionTimerExpired,
		3.f,
		false
	);
}

void AMyGrenade::Multi_Throw_Implementation()
{
	ThrowImpl();
}

void AMyGrenade::ThrowImpl()
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

void AMyGrenade::OnExplosionTimerExpired()
{
	LOG_FUNC_PRINTF(LogTemp, Warning, "Explosion Timer Expired");
	IsExploded = true;

	// todo: explosion effect

	OnExplosionTimerExpiredImpl();
}


