// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWeaponStatComponent.h"

#include "MyGameInstance.h"
#include "Utilities.hpp"

// Sets default values for this component's properties
UMyWeaponStatComponent::UMyWeaponStatComponent()
	: ID(0),
	  Damage(0),
	  MaxAmmoCount(0),
	  CurrentAmmoCount(0),
	  ClipCount(0),
	  WeaponType(EMyWeaponType::Unknown),
	  WeaponStat(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	bWantsInitializeComponent = true;
}

int32 UMyWeaponStatComponent::GetDamage() const
{
	return Damage;
}

float UMyWeaponStatComponent::GetRange() const
{
	if (WeaponType == EMyWeaponType::Range)
	{
		return GetRangeStat()->Range;
	}

	return PrintErrorAndReturnDefault<float>("Trying to get range from non-range weapon", GetOwner());
}

bool UMyWeaponStatComponent::IsHitscan() const
{
	if (WeaponType == EMyWeaponType::Range)
	{
		return GetRangeStat()->IsHitscan;
	}

	return PrintErrorAndReturnDefault<bool>("Trying to get hitscan from non-range weapon", GetOwner());
}


float UMyWeaponStatComponent::GetFireRate() const
{
	if (WeaponType == EMyWeaponType::Range)
	{
		return GetRangeStat()->FireRate;
	}

	return PrintErrorAndReturnDefault<float>("Trying to get fire rate from non-range weapon", GetOwner());
}

bool UMyWeaponStatComponent::ConsumeAmmo()
{
	if (WeaponType == EMyWeaponType::Range)
	{
		UE_LOG(LogTemp, Warning, TEXT("Consume ammo: %d"), CurrentAmmoCount);

		if (CurrentAmmoCount < 0)
		{
			CurrentAmmoCount = 0;
			return false;
		}

		--CurrentAmmoCount;
		return true;
	}
	else if (WeaponType == EMyWeaponType::Melee)
	{
		return true;
	}

	return PrintErrorAndReturnDefault<bool>("Trying to consume ammo from unknown weapon type", GetOwner());
}

// Called when the game starts
void  UMyWeaponStatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ID == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon ID is not set in %s"), *GetOwner()->GetName());
	}
}

void UMyWeaponStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	const UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());

	if (IsValid(GameInstance))
	{
		const auto& WeaponStatData = GameInstance->GetWeaponValue(ID);
		WeaponType = WeaponStatData->WeaponType;

		switch (WeaponType)
		{
		case EMyWeaponType::Melee: 
			WeaponStat = WeaponStatData->Get<FMyMeleeWeaponStat>();
			ClipCount = 0;
			CurrentAmmoCount = 0;
			MaxAmmoCount = 0;
			break;
		case EMyWeaponType::Range: 
			WeaponStat = WeaponStatData->Get<FMyRangeWeaponStat>();
			ClipCount = GetRangeStat()->Magazine;
			CurrentAmmoCount = GetRangeStat()->MaxAmmo;
			MaxAmmoCount = CurrentAmmoCount;
			break;
		case EMyWeaponType::Unknown:
		default:
			UE_LOG(LogTemp, Error, TEXT("Unknown weapon type in %s"), *GetOwner()->GetName());
		}

		Name = WeaponStatData->Name;
		Damage = WeaponStatData->Damage;
	}
}

const FMyRangeWeaponStat* UMyWeaponStatComponent::GetRangeStat() const
{
	static const auto& RangeWeaponStat = static_cast<const FMyRangeWeaponStat*>(WeaponStat);
	return RangeWeaponStat;
}

const FMyMeleeWeaponStat* UMyWeaponStatComponent::GetMeleeStat() const
{
	static const auto& MeleeWeaponStat = static_cast<const FMyMeleeWeaponStat*>(WeaponStat);
	return MeleeWeaponStat;
}

