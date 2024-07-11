// Fill out your copyright notice in the Description page of Project Settings.


#include "MyInventoryComponent.h"

#include "MyCharacter.h"
#include "MyCollectable.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UMyInventoryComponent::UMyInventoryComponent()
	: InventorySize(10)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Inventory.Reserve(InventorySize);
	Inventory.SetNum(InventorySize);
}


// Called every frame
void UMyInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UMyInventoryComponent::TryAddItem(AMyCollectable* Item)
{
	LOG_FUNC_PRINTF(LogTemp, Warning, "Trying add the item : %s", *Item->GetName())

	size_t Pos = 0;

	while (Pos < Inventory.Num() && Inventory[Pos] != nullptr)
	{
		++Pos;
	}

	if (Pos < InventorySize)
	{
		LOG_FUNC(LogTemp, Warning, "Item added to inventory");
		Inventory[Pos] = Item;
		Item->OnDestroyed.AddDynamic(this, &UMyInventoryComponent::HandleItemDestroy);
		return true;
	}
	else
	{
		LOG_FUNC(LogTemp, Warning, "Inventory is full");
		return false;
	}
}

bool UMyInventoryComponent::TryAddItem(AMyCollectable* Item, const int32 Index)
{
	LOG_FUNC_PRINTF(LogTemp, Warning, "Trying add the item : %s", *Item->GetName())

	if (Inventory.IsValidIndex(Index))
	{
		if (Inventory[Index] != nullptr)
		{
			LOG_FUNC(LogTemp, Warning, "Item already exists in the inventory");
			return false;
		}

		Inventory[Index] = Item;
		Item->OnDestroyed.AddDynamic(this, &UMyInventoryComponent::HandleItemDestroy);
		return true;
	}
	else
	{
		LOG_FUNC(LogTemp, Warning, "Index is out of bounds");
		return false;
	}
}

AMyCollectable* UMyInventoryComponent::Get(const int32 Index) const
{
	if (Inventory.IsValidIndex(Index))
	{
		return Inventory[Index].Get();
	}

	return nullptr;
}

void UMyInventoryComponent::Remove(AMyCollectable* MyCollectable)
{
	MyCollectable->OnDestroyed.RemoveDynamic(this, &UMyInventoryComponent::HandleItemDestroy);
	Inventory.RemoveSwap(MyCollectable, false);
}

bool UMyInventoryComponent::Find(AMyCollectable* MyCollectable) const
{
	return Inventory.Find(MyCollectable) != INDEX_NONE;
}

void UMyInventoryComponent::Clear()
{
	for (int32 i = 0; i < Inventory.Num(); ++i)
	{
		if (Inventory[i] != nullptr)
		{
			Inventory[i]->Destroy();
			Inventory[i] = nullptr;
		}
	}
}

void UMyInventoryComponent::DropAll()
{
	for (int32 i = 0; i < Inventory.Num(); ++i)
	{
		if (Inventory[i] != nullptr)
		{
			Inventory[i]->Server_Drop();
			Inventory[i] = nullptr;
		}
	}
}

void UMyInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMyInventoryComponent, Inventory);
	DOREPLIFETIME(UMyInventoryComponent, InventorySize);
}

void UMyInventoryComponent::HandleItemDestroy(AActor* ToBeDestroyed)
{
	const auto& Collectable = Cast<AMyCollectable>(ToBeDestroyed);
	Inventory.RemoveSwap(Collectable, false);
}

