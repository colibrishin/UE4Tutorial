// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "MyInGameWidget.generated.h"

class AMyPlayerState;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyInGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	class UMyBombIndicatorWidget* GetBombIndicatorWidget() const;

private:
	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyCharacterWidget* CharacterWidget;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyCrosshairWidget* CrosshairWidget;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyBombProgressWidget* BombProgressWidget;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyAmmoWidget* AmmoWidget;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyRoundTimeWidget* RoundTimeWidget;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyRemainingTeamWidget* RemainingTeamWidget;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyScoreWidget* ScoreWidget;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyRadarWidget* RadarWidget;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyBombIndicatorWidget* BombIndicatorWidget;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyKillFeedWidget* KillFeedWidget;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyDamageIndicatorWidget* DamageIndicatorWidget;

};
