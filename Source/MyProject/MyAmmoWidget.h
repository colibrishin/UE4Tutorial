// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum.h"

#include "Blueprint/UserWidget.h"
#include "MyAmmoWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyAmmoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateAmmo(const int32 CurrentAmmoCount, const int32 RemainingAmmoCount) const;
	void BindPlayerState(class AMyPlayerState* PlayerState) const;

private:
	void HandleStateChanged(class AMyPlayerState* PlayerState, const EMyCharacterState State) const;
	void HandleWeaponChanged(class AMyPlayerState* PlayerState) const;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* AmmoText;
};
