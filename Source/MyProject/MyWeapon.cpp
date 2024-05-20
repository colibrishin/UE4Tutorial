// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWeapon.h"

#include "MyCharacter.h"
#include "MyInventoryComponent.h"
#include "MyPlayerState.h"
#include "MyWeaponStatComponent.h"
#include "TimerManager.h"

#include "Components/BoxComponent.h"

#include "Net/UnrealNetwork.h"

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
}

void AMyWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyWeapon, ConsecutiveShots);
}

bool AMyWeapon::AttackInterruptedImpl()
{
	if (!FireRateTimerHandle.IsValid())
	{
		CanAttack = true;
	}

	if (!ReloadTimerHandle.IsValid())
	{
		CanReload = true;
	}

	return true;
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

	if (Result)
	{
		Show();
	}

	return Result;
}

void AMyWeapon::OnFireRateTimed()
{
	CanAttack = true;
	OnFireReady.Broadcast();
	GetWorld()->GetTimerManager().ClearTimer(FireRateTimerHandle);
}

void AMyWeapon::OnReloadDone()
{
	CanAttack = true;
	CanReload = true;
	OnReloadReady.Broadcast();
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
}

void AMyWeapon::OnCookingTimed()
{
	CanAttack = false;
	CanReload = false;
	GetWorld()->GetTimerManager().ClearTimer(CookingTimerHandle);
}

void AMyWeapon::DropImpl()
{
	Super::DropImpl();

	if (HasAuthority())
	{
		if (const auto& PlayerState = GetItemOwner()->GetPlayerState<AMyPlayerState>())
		{
			PlayerState->SetCurrentWeapon(nullptr);
		}
	}
}

bool AMyWeapon::Attack()
{
	if (CanAttack)
	{
		switch (GetWeaponStatComponent()->GetWeaponType())
		{
		case EMyWeaponType::Range: 
			GetWorld()->GetTimerManager().SetTimer
				(
				 FireRateTimerHandle,
				 this,
				 &AMyWeapon::OnFireRateTimed,
				 GetWeaponStatComponent()->GetFireRate(),
				 false
				);

			if (HasAuthority())
			{
				++ConsecutiveShots;
			}
			break;
		case EMyWeaponType::Melee:
			LOG_FUNC(LogTemp, Warning, "Melee attack, Not implemented");
			return AttackImpl();
		case EMyWeaponType::Throwable: 
			GetWorld()->GetTimerManager().SetTimer
				(
				 CookingTimerHandle,
				 this,
				 &AMyWeapon::OnCookingTimed,
				 GetWeaponStatComponent()->GetCookingTime(),
				 false
				);
			return AttackImpl();
		case EMyWeaponType::Unknown:
		default:
			return AttackImpl();
		}

		CanAttack = false;

		return AttackImpl();
	}

	return false;
}

bool AMyWeapon::AttackInterrupted()
{
	LOG_FUNC(LogTemp, Warning, "AttackInterrupted");
	ConsecutiveShots = 0;
	return AttackInterruptedImpl();
}

bool AMyWeapon::Reload()
{
	if (CanReload)
	{
		switch (GetWeaponStatComponent()->GetWeaponType())
		{
		case EMyWeaponType::Range: 
			GetWorld()->GetTimerManager().SetTimer
				(
				 ReloadTimerHandle,
				 this,
				 &AMyWeapon::OnReloadDone,
				 GetWeaponStatComponent()->GetReloadTime(),
				 false
				);
			break;
		case EMyWeaponType::Unknown:
		case EMyWeaponType::Melee:
		case EMyWeaponType::Throwable:
		default:
			return ReloadImpl();
		}

		CanAttack = false;
		CanReload = false;

		return ReloadImpl();
	}

	return false;
}
