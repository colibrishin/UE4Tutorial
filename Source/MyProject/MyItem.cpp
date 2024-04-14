// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyItem.h"

#include "MyCharacter.h"
#include "MyInventoryComponent.h"

AMyItem::AMyItem()
{
}

bool AMyItem::PostInteract(AMyCharacter* Character)
{
	const auto& Inventory = Character->GetInventory();

	if (Inventory->TryAddItem(this))
	{
		UE_LOG(LogTemp, Warning, TEXT("Item Interacted: %s"), *GetName());
		return true && Super::PostInteract(Character);;
	}
	else
	{
		return false && Super::PostInteract(Character);
	}

	return false && Super::PostInteract(Character);
}

void AMyItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsValid(GetMesh()))
	{
		UE_LOG(LogTemp, Error, TEXT("Item Mesh is not valid"));
	}
}
