// Fill out your copyright notice in the Description page of Project Settings.


#include "MyInventoryComponent.h"

#include "MyCharacter.h"
#include "MyCollectable.h"

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
	if (Inventory.Num() < InventorySize)
	{
		Inventory.Add(Item);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory is full"));
		return false;
	}
}

