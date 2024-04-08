// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyItem.h"

#include "MyCharacter.h"
#include "MyInventoryComponent.h"

AMyItem::AMyItem()
{
	GetMesh()->SetSimulatePhysics(true);
}

bool AMyItem::InteractImpl(AMyCharacter* Character)
{
	const auto& Inventory = Character->GetInventory();
	return Inventory->TryAddItem(this);
}

void AMyItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsValid(GetMesh()))
	{
		UE_LOG(LogTemp, Error, TEXT("Item Mesh is not valid"));
	}
}
