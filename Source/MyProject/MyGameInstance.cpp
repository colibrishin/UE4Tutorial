// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "Data.h"
#include "MyCharacter.h"
#include "MyStatComponent.h"
#include "MyWeapon.h"
#include "MyWeaponDataAsset.h"
#include "Utilities.hpp"

#include "UObject/ConstructorHelpers.h"

UMyGameInstance::UMyGameInstance()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_STAT(TEXT("DataTable'/Game/Data/StatsData.StatsData'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_WEAPON_STAT(TEXT("DataTable'/Game/Data/WeaponData.WeaponData'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_COLLECTABLE_STAT(TEXT("DataTable'/Game/Data/CollectableData.CollectableData'"));

	StatTable = DT_STAT.Object;
	WeaponStatTable = DT_WEAPON_STAT.Object;
    CollectableDataTable = DT_COLLECTABLE_STAT.Object;
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

void UMyGameInstance::GetCollectableValue(const int32 ID, FMyCollectableData** const OutData) const
{
	const auto Row = CollectableDataTable->FindRow<FMyCollectableData>(*FString::FromInt(ID), TEXT(""));
	*OutData = Row;
}
