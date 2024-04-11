// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyItem.h"

#include "MyCharacter.h"
#include "MyInventoryComponent.h"

AMyItem::AMyItem()
{
	GetMesh()->SetSimulatePhysics(true);
}

bool AMyItem::Interact(AMyCharacter* Character)
{
	if (Super::Interact(Character))
	{
		UE_LOG(LogTemp, Warning, TEXT("Item Interacted: %s"), *GetName());
		const auto& Inventory = Character->GetInventory();
		return Inventory->TryAddItem(this);
	}

	Drop();
	return false;
}

void AMyItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsValid(GetMesh()))
	{
		UE_LOG(LogTemp, Error, TEXT("Item Mesh is not valid"));
	}
}
