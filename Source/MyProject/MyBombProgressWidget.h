// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum.h"

#include "Blueprint/UserWidget.h"
#include "MyBombProgressWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyBombProgressWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetValue(const float Value) const;
	void BindGameState(class AMyGameState* GameState);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void OnBombStateChanged(const EMyBombState NewState);

	UPROPERTY(Meta=(BindWidget))
	class UProgressBar* ProgressBar;

	bool bNeedToShow;
};
