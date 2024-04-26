// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWeapon.h"

#include "MyCharacter.h"
#include "MyPlayerState.h"
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

bool AMyWeapon::TryAttachItem(const AMyCharacter* Character)
{
	if (GetMesh()->AttachToComponent
		(
		 Character->GetMesh(),
		 FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		 AMyCharacter::LeftHandSocketName
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

bool AMyWeapon::PostInteract(AMyCharacter* Character)
{
	const auto& Result = Super::PostInteract(Character);

	if (Character->TryPickWeapon(this))
	{
		Show();

		return Result && true;
	}

	return Result && false;
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
		if (GetWeaponStatComponent()->GetWeaponType() == EMyWeaponType::Range)
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
		else if (GetWeaponStatComponent()->GetWeaponType() == EMyWeaponType::Melee)
		{
			LOG_FUNC(LogTemp, Warning, "Melee attack, Not implemented");
			return AttackImpl();
		}

		CanAttack = false;

		return AttackImpl();
	}

	return false;
}

bool AMyWeapon::Reload()
{
	if (CanReload)
	{
		if (GetWeaponStatComponent()->GetWeaponType() == EMyWeaponType::Range)
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
		else if (GetWeaponStatComponent()->GetWeaponType() == EMyWeaponType::Melee)
		{
			LOG_FUNC(LogTemp, Warning, "Melee reload, Not implemented");
			return ReloadImpl();
		}

		CanReload = false;

		return ReloadImpl();
	}

	return false;
}

bool AMyWeapon::Drop()
{
	const auto& ItemOwner = GetItemOwner();
	const auto& Result = Super::Drop();

	if (Result)
	{
		if (HasAuthority())
		{
			if (const auto& PlayerState = ItemOwner->GetPlayerState<AMyPlayerState>())
			{
				PlayerState->SetWeapon(nullptr);
			}
		}
		
		return true && Result;
	}

	return false && Result;
}
