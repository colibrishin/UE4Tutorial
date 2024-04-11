// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyInGameHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyInGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMyInGameHUD();

	void BindBomb(class AMyC4* Bomb) const;

protected:
	virtual void BeginPlay() override;

	virtual void DrawHUD() override;

	virtual void PostInitializeComponents() override;


private:
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* Widgets;

};
