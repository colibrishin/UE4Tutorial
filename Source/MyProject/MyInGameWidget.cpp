// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyInGameWidget.h"

#include "MyAmmoWidget.h"
#include "MyBombProgressWidget.h"
#include "MyC4.h"
#include "MyCharacter.h"
#include "MyCharacterWidget.h"

void UMyInGameWidget::BindPlayer(const AMyCharacter* Player) const
{
	if (IsValid(Player))
	{
		if (CharacterWidget)
		{
			CharacterWidget->BindHp(Player->GetStatComponent());
		}
	}
}

void UMyInGameWidget::BindBomb(AMyC4* Bomb) const
{
	if (IsValid(Bomb))
	{
		if (BombProgressWidget)
		{
			BombProgressWidget->BindBomb(Bomb);
		}
	}
}

void UMyInGameWidget::UpdateAmmo(const int32 CurrentAmmoCount, const int32 ClipCount, const int32 MaxAmmoCount) const
{
	if (IsValid(AmmoWidget))
	{
		AmmoWidget->UpdateAmmo(CurrentAmmoCount, ClipCount, MaxAmmoCount);
	}
}
