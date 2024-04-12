// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWeapon.h"

#include "MyCharacter.h"
#include "MyWeaponStatComponent.h"

#include "Components/BoxComponent.h"

// Sets default values
AMyWeapon::AMyWeapon() : CanReload(true), CanAttack(true)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponStatComponent = CreateDefaultSubobject<UMyWeaponStatComponent>(TEXT("WeaponStatComponent"));
}

// Called when the game starts or when spawned
void AMyWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void AMyWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

  if (GetMesh()->GetStaticMesh() == nullptr)
  {
	  UE_LOG(LogTemp, Error, TEXT("Weapon mesh is not set"));
  }
}

void AMyWeapon::OnFireRateTimed()
{
	CanAttack = true;
	OnFireReady.Broadcast();
	GetWorld()->GetTimerManager().ClearTimer(FireRateTimerHandle);
}

void AMyWeapon::OnReloadDone()
{
	CanReload = true;
	OnReloadReady.Broadcast();
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
}

bool AMyWeapon::Attack()
{
	if (CanAttack)
	{
		if (GetWeaponStatComponent()->GetWeaponType() == Range)
		{
			GetWorld()->GetTimerManager().SetTimer
			(
				FireRateTimerHandle, 
				this, 
				&AMyWeapon::OnFireRateTimed, 
				GetWeaponStatComponent()->GetFireRate(), 
				false
			);
		}
		else if (GetWeaponStatComponent()->GetWeaponType() == Melee)
		{
			LOG_FUNC(LogTemp, Warning, "Melee attack, Not implemented");
			return AttackImpl();
		}

		CanAttack = false;

		return AttackImpl();
	}

	return false;
}

bool AMyWeapon::Interact(AMyCharacter* Character)
{
	if (Super::Interact(Character))
	{
		if (Character->TryPickWeapon(this))
		{
			return true;
		}
	}

	Drop();
	return false;
}

bool AMyWeapon::Reload()
{
	if (CanReload)
	{
		if (GetWeaponStatComponent()->GetWeaponType() == Range)
		{
			GetWorld()->GetTimerManager().SetTimer
			(
				 ReloadTimerHandle ,
				 this ,
				 &AMyWeapon::OnReloadDone ,
				 GetWeaponStatComponent()->GetReloadTime() ,
				 false
			);
		}
		else if (GetWeaponStatComponent()->GetWeaponType() == Melee)
		{
			LOG_FUNC(LogTemp, Warning, "Melee reload, Not implemented");
			return ReloadImpl();
		}

		CanReload = false;

		return ReloadImpl();
	}

	return false;
}
