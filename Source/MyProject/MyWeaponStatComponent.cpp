// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWeaponStatComponent.h"

#include "Data.h"
#include "MyCharacter.h"
#include "MyGameInstance.h"
#include "MyPlayerState.h"
#include "MyWeaponDataAsset.h"
#include "Utilities.hpp"

// Sets default values for this component's properties
UMyWeaponStatComponent::UMyWeaponStatComponent()
	: ID(0),
	  Damage(0),
	  AmmoSpent(0),
	  LoadedAmmoCount(0),
	  AmmoPerLoad(0),
	  TotalAmmoCount(0),
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

float UMyWeaponStatComponent::GetCookingTime() const
{
	if (WeaponType == EMyWeaponType::Throwable)
	{
		return GetThrowableStat()->CookingTime;
	}

	return PrintErrorAndReturnDefault<float>("Trying to get cooking time from non-throwable weapon", GetOwner());
}

float UMyWeaponStatComponent::GetRadius() const
{
	if (WeaponType == EMyWeaponType::Melee)
	{
		return GetMeleeStat()->Radius;
	}
	else if (WeaponType == EMyWeaponType::Throwable)
	{
		return GetThrowableStat()->Radius;
	}

	return PrintErrorAndReturnDefault<float>("Trying to get radius from non-melee and non-throwable weapon", GetOwner());
}


float UMyWeaponStatComponent::GetVSpread() const
{
	if (WeaponType == EMyWeaponType::Range)
	{
		return GetRangeStat()->VSpread;
	}

	return PrintErrorAndReturnDefault<float>("Trying to get vspread from non-range weapon", GetOwner());
}

float UMyWeaponStatComponent::GetHSpread() const
{
	if (WeaponType == EMyWeaponType::Range)
	{
		return GetRangeStat()->HSpread;
	}

	return PrintErrorAndReturnDefault<float>("Trying to get hspread from non-range weapon", GetOwner());
}

bool UMyWeaponStatComponent::ConsumeAmmo()
{
	switch (WeaponType)
	{
	case EMyWeaponType::Range:
		{
			LOG_FUNC_PRINTF(LogTemp , Warning , "Consume ammo: %d" , AmmoSpent);

			if (LoadedAmmoCount - AmmoSpent <= 0)
			{
				AmmoSpent = LoadedAmmoCount;
				return false;
			}

			++AmmoSpent;
			return true;
		}
	case EMyWeaponType::Melee:
		{
			UE_LOG(LogTemp, Warning, TEXT("%hs: %s"), __FUNCTION__, *FString::Printf(TEXT("Melee weapon does not need to consume ammo")));
			return true;
		}
	case EMyWeaponType::Throwable:
		{
			UE_LOG(LogTemp, Warning, TEXT("%hs: %s"), __FUNCTION__, *FString::Printf(TEXT("Throwable weapon does not need to consume ammo")));
			return true;
		}
	case EMyWeaponType::Unknown:
	default: break;
	}

	return PrintErrorAndReturnDefault<bool>("Trying to consume ammo from unknown weapon type", GetOwner());
}

void UMyWeaponStatComponent::Reload()
{
	switch (WeaponType)
	{
	case EMyWeaponType::Range:
		{
			if (TotalAmmoCount <= 0)
			{
				LOG_FUNC(LogTemp , Warning , "No ammo to reload");
				return;
			}

			TotalAmmoCount -= AmmoSpent;

			if (TotalAmmoCount <= AmmoPerLoad)
			{
				LoadedAmmoCount = TotalAmmoCount;
			}
			else
			{
				LoadedAmmoCount = AmmoPerLoad;
			}

			AmmoSpent = 0;
			break;
		}
	case EMyWeaponType::Melee:
		{
			LOG_FUNC(LogTemp, Warning, "Melee weapon does not need to reload");
			break;
		}
	case EMyWeaponType::Throwable:
		{
			LOG_FUNC(LogTemp, Warning, "Throwable weapon does not need to reload");
			break;
		}
	case EMyWeaponType::Unknown:
	default:
		{
			LOG_FUNC_PRINTF(LogTemp, Error , "Unknown weapon type in %s" , *GetOwner()->GetName());
			break;
		}
	}
}

float UMyWeaponStatComponent::GetReloadTime() const
{
	if (WeaponType == EMyWeaponType::Range)
	{
		return GetRangeStat()->ReloadTime;
	}

	return PrintErrorAndReturnDefault<float>("Trying to get reload time from non-range weapon", GetOwner());
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

	if (ID == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon ID is not set in %s"), *GetOwner()->GetName());
		return;
	}

	const auto& WeaponData = GetRowData<FMyWeaponData>(this, ID);

	if (WeaponData == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon data is not valid in %s"), *GetOwner()->GetName());
		return;
	}

	const auto& WeaponStatData = WeaponData->WeaponDataAsset->GetWeaponStat();

	WeaponType = WeaponStatData.WeaponType;

	switch (WeaponType)
	{
	case EMyWeaponType::Melee: 
		WeaponStat = WeaponStatData.Get<FMyMeleeWeaponStat>();
		LoadedAmmoCount = 0;
		AmmoSpent = 0;
		AmmoPerLoad = 0;
		TotalAmmoCount = 0;
		break;
	case EMyWeaponType::Range: 
		WeaponStat = WeaponStatData.Get<FMyRangeWeaponStat>();
		LoadedAmmoCount = GetRangeStat()->MaxAmmo;
		AmmoSpent = 0;
		AmmoPerLoad = LoadedAmmoCount;
		TotalAmmoCount = (GetRangeStat()->MaxAmmo * GetRangeStat()->Magazine) + LoadedAmmoCount;
		break;
	case EMyWeaponType::Throwable:
		WeaponStat = WeaponStatData.Get<FMyThrowableWeaponStat>();
		LoadedAmmoCount = 0;
		AmmoSpent = 0;
		AmmoPerLoad = 0;
		TotalAmmoCount = 0;
		break;
	case EMyWeaponType::Unknown:
	default:
		LOG_FUNC_PRINTF(LogTemp, Error, "Unknown weapon type in %s", *GetOwner()->GetName());
	}

	Name = WeaponStatData.Name;
	Damage = WeaponStatData.Damage;
}

const FMyRangeWeaponStat* UMyWeaponStatComponent::GetRangeStat() const
{
	const auto& RangeWeaponStat = static_cast<const FMyRangeWeaponStat*>(WeaponStat);
	return RangeWeaponStat;
}

const FMyMeleeWeaponStat* UMyWeaponStatComponent::GetMeleeStat() const
{
	const auto& MeleeWeaponStat = static_cast<const FMyMeleeWeaponStat*>(WeaponStat);
	return MeleeWeaponStat;
}

const FMyThrowableWeaponStat* UMyWeaponStatComponent::GetThrowableStat() const
{
	const auto& ThrowableWeaponStat = static_cast<const FMyThrowableWeaponStat*>(WeaponStat);
	return ThrowableWeaponStat;
}
