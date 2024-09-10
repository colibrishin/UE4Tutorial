// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

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
