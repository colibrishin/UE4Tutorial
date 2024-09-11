// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWeapon.h"

#include "MyCharacter.h"
#include "MyProject/Components/MyInventoryComponent.h"
#include "MyPlayerState.h"
#include "MyWeaponDataAsset.h"
#include "MyProject/Components/MyWeaponStatComponent.h"
#include "TimerManager.h"

#include "Components/BoxComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AMyWeapon::AMyWeapon() : CanReload(true), CanAttack(true)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponStatComponent = CreateDefaultSubobject<UMyWeaponStatComponent>(TEXT("WeaponStatComponent"));
	WeaponStatComponent->SetNetAddressable();
	WeaponStatComponent->SetIsReplicated(true);
	AddOwnedComponent(WeaponStatComponent);
}

int32 AMyWeapon::GetDamage() const
{
	return WeaponStatComponent->GetDamage();
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
	DOREPLIFETIME(AMyWeapon, WeaponStatComponent);
	DOREPLIFETIME(AMyWeapon, ConsecutiveShots);
	DOREPLIFETIME(AMyWeapon, bIsDummyVisually);
}

bool AMyWeapon::AttackInterruptedImpl()
{
	if (!OnFireReadyTimerHandle.IsValid())
	{
		CanAttack = true;
	}

	if (!OnReloadDoneTimerHandle.IsValid())
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

		MyCharacter->OnInteractInterrupted.AddUniqueDynamic(this, &AMyCollectable::Server_InteractInterrupted);
		MyCharacter->OnUseInterrupted.AddUniqueDynamic(this, &AMyCollectable::Server_UseInterrupted);

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

void AMyWeapon::ApplyAssets(UMyWeaponDataAsset* InAssetData)
{
	GetWeaponStatComponent()->SetID(InAssetData->GetID());
}

void AMyWeapon::OnFireRateTimed()
{
	CanAttack = true;
	OnFireReady.Broadcast();
	OnFireReadyTimerHandle.Invalidate();
}

void AMyWeapon::OnReloadDone()
{
	CanAttack = true;
	CanReload = true;
	OnReloadReady.Broadcast();
	OnFireReadyTimerHandle.Invalidate();
}

void AMyWeapon::OnCookingTimed()
{
	CanAttack = false;
	CanReload = false;
	OnCookingTimerHandle.Invalidate();
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

void AMyWeapon::Client_PlayAttackSound_Implementation()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation(), GetActorRotation());
	}
}

void AMyWeapon::Client_PlayReloadSound_Implementation()
{
	if (ReloadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation(), GetActorRotation());
	}
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
