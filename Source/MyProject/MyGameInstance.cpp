// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

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

FMyStat* UMyGameInstance::GetValue(const int32 Level) const
{
	// FName은 대소문자를 구별하지 않는다.
	return StatTable->FindRow<FMyStat>(*FString::FromInt(Level), TEXT(""));
}

FMyWeaponStat* UMyGameInstance::GetWeaponValue(const int32 ID) const
{
	return WeaponStatTable->FindRow<FMyWeaponStat>(*FString::FromInt(ID), TEXT(""));
}
