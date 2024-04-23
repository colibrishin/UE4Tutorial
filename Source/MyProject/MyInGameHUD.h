// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPlayerState.h"

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

	void UpdateAmmo(int32 CurrentAmmoCount, const int32 RemainingAmmoCount) const;

	class UMyInGameWidget* GetInGameWidget() const;

	bool IsBuyMenuOpened() const;
	void SetState(class AMyPlayerState* MyPlayerState) const;

protected:
	virtual void BeginPlay() override;

	virtual void DrawHUD() override;

	virtual void PostInitializeComponents() override;


private:
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* Widgets;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* BuyMenu;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* StatWidget;

};
