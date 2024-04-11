// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyInGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyInGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindPlayer(const class AMyCharacter* Player) const;
	void BindBomb(class AMyC4* Bomb) const;
	void UpdateAmmo(const int32 CurrentAmmoCount, const int32 ClipCount, const int32 MaxAmmoCount) const;

private:
	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyCharacterWidget* CharacterWidget;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyCrosshairWidget* CrosshairWidget;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyBombProgressWidget* BombProgressWidget;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyAmmoWidget* AmmoWidget;

};
