// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyItem.h"

#include "MyCharacter.h"
#include "MyInventoryComponent.h"
#include "MyPlayerState.h"

AMyItem::AMyItem()
{
}

void AMyItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMyItem::DropBeforeCharacter()
{
	Super::DropBeforeCharacter();

	if (HasAuthority())
	{
		if (const auto& PlayerState = GetItemOwner()->GetPlayerState<AMyPlayerState>())
		{
			PlayerState->SetCurrentItem(nullptr);
		}
	}
}
