// Fill out your copyright notice in the Description page of Project Settings.


#include "C_CollectableAsset.h"

#include "MyProject/DataAsset/DA_Collectable.h"


// Sets default values for this component's properties
UC_CollectableAsset::UC_CollectableAsset()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

EMySlotType UC_CollectableAsset::GetSlot() const
{
	if (const UDA_Collectable* Collectable = GetAsset<UDA_Collectable>())
	{
		return Collectable->GetSlotType();
	}

	return EMySlotType::Unknown;
}

FString UC_CollectableAsset::GetAssetName() const
{
	if (const UDA_Collectable* Collectable = GetAsset<UDA_Collectable>())
	{
		return Collectable->GetAssetName();
	}

	return TEXT("");
}

void UC_CollectableAsset::ApplyAsset()
{
	Super::ApplyAsset();
}


// Called when the game starts
void UC_CollectableAsset::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


