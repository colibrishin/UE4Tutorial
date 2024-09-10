// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyInGameWidget.h"

#include "MyAmmoWidget.h"
#include "MyBombIndicatorWidget.h"
#include "MyBombProgressWidget.h"
#include "MyCharacterWidget.h"
#include "MyCrosshairWidget.h"
#include "MyKillFeedWidget.h"
#include "MyRadarWidget.h"
#include "MyRemainingTeamWidget.h"
#include "MyRoundTimeWidget.h"
#include "MyScoreWidget.h"

UMyInGameWidget::UMyInGameWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CharacterWidget = CreateDefaultSubobject<UMyCharacterWidget>(TEXT("CharacterWidget"));
	CrosshairWidget = CreateDefaultSubobject<UMyCrosshairWidget>(TEXT("CrosshairWidget"));
	BombProgressWidget = CreateDefaultSubobject<UMyBombProgressWidget>(TEXT("BombProgressWidget"));
	AmmoWidget = CreateDefaultSubobject<UMyAmmoWidget>(TEXT("AmmoWidget"));
	RoundTimeWidget = CreateDefaultSubobject<UMyRoundTimeWidget>(TEXT("RoundTimeWidget"));
	RemainingTeamWidget = CreateDefaultSubobject<UMyRemainingTeamWidget>(TEXT("RemainingTeamWidget"));
	ScoreWidget = CreateDefaultSubobject<UMyScoreWidget>(TEXT("ScoreWidget"));
	RadarWidget = CreateDefaultSubobject<UMyRadarWidget>(TEXT("RadarWidget"));
	BombIndicatorWidget = CreateDefaultSubobject<UMyBombIndicatorWidget>(TEXT("BombIndicatorWidget"));
	KillFeedWidget = CreateDefaultSubobject<UMyKillFeedWidget>(TEXT("KillFeedWidget"));
	DamageIndicatorWidget = CreateDefaultSubobject<UMyDamageIndicatorWidget>(TEXT("DamageIndicatorWidget"));
}

UMyBombIndicatorWidget* UMyInGameWidget::GetBombIndicatorWidget() const
{
	return BombIndicatorWidget;
}

void UMyInGameWidget::BindPlayerState(AMyPlayerState* MyPlayerState) const
{
	if (IsValid(CharacterWidget))
	{
		CharacterWidget->BindPlayerState(MyPlayerState);
	}
	
	if (IsValid(DamageIndicatorWidget))
	{
		DamageIndicatorWidget->BindPlayerState(MyPlayerState);
	}
}
