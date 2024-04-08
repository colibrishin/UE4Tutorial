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

private:
	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyCharacterWidget* CharacterWidget;

	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
	class UMyCrosshairWidget* CrosshairWidget;
};
