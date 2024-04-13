// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "Data.h"
#include "MyCharacter.h"
#include "MyStatComponent.h"
#include "MyWeapon.h"
#include "MyWeaponDataAsset.h"
#include "Utilities.hpp"

UMyGameInstance::UMyGameInstance()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_STAT(TEXT("DataTable'/Game/Data/StatsData.StatsData'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_WEAPON_STAT(TEXT("DataTable'/Game/Data/WeaponData.WeaponData'"));

	StatTable = DT_STAT.Object;
	WeaponStatTable = DT_WEAPON_STAT.Object;
}

void UMyGameInstance::Init()
{
	Super::Init();
}

void UMyGameInstance::GetStatValue(const int32 Level, FMyStat** OutStat) const
{
	const auto Row = StatTable->FindRow<FMyStat>(*FString::FromInt(Level), TEXT(""));
	*OutStat = Row;
}
void UMyGameInstance::GetWeaponValue(const int32 ID, FMyWeaponData** OutData) const
{
	const auto Row = WeaponStatTable->FindRow<FMyWeaponData>(*FString::FromInt(ID), TEXT(""));
	*OutData = Row;
}

bool UMyGameInstance::BuyWeapon_Validate(AMyCharacter* Character , const int32 WeaponID) const
{
	if (!IsValid(Character))
	{
		LOG_FUNC(LogTemp, Error, "Player is invalid");
		return false;
	}

	const auto& WeaponData = GetWeaponData(this, WeaponID);
	const auto& WeaponStat = WeaponData->WeaponDataAsset->GetWeaponStat();

	if (WeaponData == nullptr)
	{
		LOG_FUNC(LogTemp, Error, "WeaponInfo is nullptr");
		return false;
	}

	if (Character->GetStatComponent()->GetMoney() <= 0)
	{
		LOG_FUNC(LogTemp, Error, "Player => Not enough money");
		return false;
	}

	if (Character->GetStatComponent()->GetMoney() - WeaponStat.Price < 0)
	{
		LOG_FUNC(LogTemp, Error, "Player has money but not enough money to buy");
		return false;
	}

	return true;
}

void UMyGameInstance::Multi_BuyWeapon_Implementation(TSubclassOf<class AMyWeapon> WeaponClass, AMyCharacter* Character, const int32 ID) const
{
	FActorSpawnParameters SpawnParams;

	SpawnParams.Owner = Character;
	SpawnParams.Instigator = Character;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.ObjectFlags = RF_Transient;
	SpawnParams.bNoFail = true;

	const auto& GeneratedWeapon = GetWorld()->SpawnActor<AMyWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	LOG_FUNC_RAW(LogTemp, Warning, *FString::Printf(TEXT("%s"), *WeaponClass->GetName()));

	if (IsValid(GeneratedWeapon))
	{
		// todo: force to player to pick weapon.
		GeneratedWeapon->SetReplicates(true);
		GeneratedWeapon->Interact(Character);
	}
}

void UMyGameInstance::Server_BuyWeapon_Implementation(AMyCharacter* Character, const int32 WeaponID) const
{
	if (!IsValid(Character))
	{
		LOG_FUNC(LogTemp, Error, "Player is invalid");
		return;
	}

	const auto& WeaponData = GetWeaponData(this, WeaponID);

	Multi_BuyWeapon(WeaponData->WeaponDataAsset->GetWeaponClass(), Character, WeaponID);
}

bool UMyGameInstance::Server_BuyWeapon_Validate(AMyCharacter* Character, const int32 WeaponID)
{
	return BuyWeapon_Validate(Character, WeaponID);
}

void UMyGameInstance::BuyWeapon(AMyCharacter* Character, const int32 WeaponID) const
{
	if (!BuyWeapon_Validate(Character, WeaponID))
	{
		return;
	}

	if (IsValid(Character))
	{
		Server_BuyWeapon(Character, WeaponID);
	}
}
