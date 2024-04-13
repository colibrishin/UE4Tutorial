// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "Data.h"

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
