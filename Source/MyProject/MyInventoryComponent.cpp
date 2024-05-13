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

	// ...
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

	if (Inventory.Num() < InventorySize)
	{
		LOG_FUNC(LogTemp, Warning, "Item added to inventory");
		Inventory.Add(Item);
		Item->OnDestroyed.AddDynamic(this, &UMyInventoryComponent::HandleItemDestroy);
		return true;
	}
	else
	{
		LOG_FUNC(LogTemp, Warning, "Inventory is full");
		return false;
	}
}

AMyCollectable* UMyInventoryComponent::Use(const int32 Index)
{
	if (Index < Inventory.Num())
	{
		AMyCollectable* Item = Inventory[Index].Get();

		if (Item)
		{
			Item->OnDestroyed.RemoveDynamic(this, &UMyInventoryComponent::HandleItemDestroy);
			Inventory.RemoveAt(Index);
			return Item;
		}

		LOG_FUNC(LogTemp, Warning, "Item is not valid");
		return nullptr;
	}
	else
	{
		UE_LOG(LogTemp , Warning , TEXT("Item not found"));
		return nullptr;
	}
}

void UMyInventoryComponent::Remove(AMyCollectable* MyCollectable)
{
	Inventory.Remove(MyCollectable);
}

bool UMyInventoryComponent::Find(AMyCollectable* MyCollectable) const
{
	return Inventory.Find(MyCollectable) != INDEX_NONE;
}

void UMyInventoryComponent::Clear()
{
	for (int32 i = 0; i < Inventory.Num(); ++i)
	{
		Inventory[i]->Destroy();
		Inventory[i] = nullptr;
	}
}

void UMyInventoryComponent::DropAll()
{
	for (int32 i = 0; i < Inventory.Num(); ++i)
	{
		Inventory[i]->Drop();
		Inventory[i] = nullptr;
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
	Inventory.Remove(Collectable);
}

