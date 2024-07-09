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

bool AMyWeapon::TryAttachItem(AMyCharacter* Character)
{
	if (GetMesh()->AttachToComponent
		(
		 Character->GetMesh(),
		 FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		 AMyCharacter::LeftHandSocketName
		))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("AttachToComponent success"));

		const auto& MyCharacter = GetItemOwner();

		OnInteractInterruptedHandle = MyCharacter->BindOnInteractInterrupted(this, &AMyCollectable::Server_InteractInterrupted);
		OnUseInterruptedHandle = MyCharacter->BindOnUseInterrupted(this, &AMyCollectable::Server_UseInterrupted);

		Client_TryAttachItem(Character);

		return true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("AttachToComponent failed"));
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

void AMyWeapon::DropBeforeCharacter()
{
	Super::DropBeforeCharacter();

	if (HasAuthority())
	{
		if (const auto& PlayerState = GetItemOwner()->GetPlayerState<AMyPlayerState>())
		{
			PlayerState->SetCurrentWeapon(nullptr);
		}
	}
}

void AMyWeapon::Client_CookThrowable_Implementation()
{
	GetWorld()->GetTimerManager().SetTimer
				(
				 CookingTimerHandle,
				 this,
				 &AMyWeapon::OnCookingTimed,
				 GetWeaponStatComponent()->GetCookingTime(),
				 false
				);
}

void AMyWeapon::Client_AttackRange_Implementation()
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

void AMyWeapon::Client_ReloadRange_Implementation()
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

bool AMyWeapon::Attack()
{
	if (CanAttack)
	{
		switch (GetWeaponStatComponent()->GetWeaponType())
		{
		case EMyWeaponType::Range: 
			break;
		case EMyWeaponType::Melee:
		case EMyWeaponType::Throwable: 
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
	// todo: Consecutive shot decay
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
