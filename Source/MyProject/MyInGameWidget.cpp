// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyInGameWidget.h"

#include "MyAmmoWidget.h"
#include "MyBombProgressWidget.h"
#include "MyC4.h"
#include "MyCharacter.h"
#include "MyCharacterWidget.h"
#include "MyGameState.h"
#include "MyRemainingTeamWidget.h"
#include "MyRoundTimeWidget.h"
#include "MyScoreWidget.h"

void UMyInGameWidget::UpdateAmmo(const int32 CurrentAmmoCount, const int32 RemainingAmmoCount) const
{
	if (IsValid(AmmoWidget))
	{
		AmmoWidget->UpdateAmmo(CurrentAmmoCount, RemainingAmmoCount);
	}
}

UMyBombIndicatorWidget* UMyInGameWidget::GetBombIndicatorWidget() const
{
	return BombIndicatorWidget;
}
