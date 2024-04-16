// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyInGameWidget.h"

#include "MyAmmoWidget.h"
#include "MyBombProgressWidget.h"
#include "MyC4.h"
#include "MyCharacter.h"
#include "MyCharacterWidget.h"
#include "MyRoundTimeWidget.h"

void UMyInGameWidget::BindPlayer(AMyPlayerState* State) const
{
	if (IsValid(State))
	{
		if (CharacterWidget)
		{
			CharacterWidget->BindHp(State);
		}
	}
}

void UMyInGameWidget::BindBomb(const AMyC4* Bomb) const
{
	if (IsValid(Bomb))
	{
		if (BombProgressWidget)
		{
			BombProgressWidget->BindBomb(Bomb);
		}
	}
}

void UMyInGameWidget::UpdateAmmo(const int32 CurrentAmmoCount, const int32 RemainingAmmoCount) const
{
	if (IsValid(AmmoWidget))
	{
		AmmoWidget->UpdateAmmo(CurrentAmmoCount, RemainingAmmoCount);
	}
}
