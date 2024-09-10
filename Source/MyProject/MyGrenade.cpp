// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyGrenade.h"

#include "MyCharacter.h"
#include "MyWeaponStatComponent.h"

bool AMyGrenade::AttackImpl()
{
	if (!OnCookingTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer
				(
				 OnCookingTimerHandle ,
				 this ,
				 &AMyWeapon::OnCookingTimed ,
				 GetWeaponStatComponent()->GetCookingTime() ,
				 false
				);

		Client_CookThrowable();
	}
	
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

void AMyGrenade::Client_CookThrowable_Implementation()
{
	GetWorld()->GetTimerManager().SetTimer
			(
			 OnCookingTimerHandle ,
			 this ,
			 &AMyWeapon::OnCookingTimed ,
			 GetWeaponStatComponent()->GetCookingTime() ,
			 false
			);
}

void AMyGrenade::Throw()
{
	if (IsExploded || IsThrown)
	{
		return;
	}

	IsThrown = true;

	PreviousOwner = GetItemOwner();

	GetSkeletalMeshComponent()->AddImpulse(FVector::ForwardVector * 1000.f, NAME_None, true);
	Server_Drop();
}

void AMyGrenade::Charge()
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

void AMyGrenade::OnExplosionTimerExpired()
{
	UE_LOG(LogTemp, Warning, TEXT("%hs: %s"), __FUNCTION__, *FString::Printf(TEXT("Explosion Timer Expired")));
	IsExploded = true;

	// todo: explosion effect

	OnExplosionTimerExpiredImpl();
}


