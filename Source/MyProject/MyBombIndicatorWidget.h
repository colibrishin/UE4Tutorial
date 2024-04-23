// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum.h"
#include "MyC4.h"

#include "Blueprint/UserWidget.h"
#include "MyBombIndicatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyBombIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Reset();

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void OnBombProgressChanged(const EMyBombState State);

	void OnBombPicked(class AMyCharacter* Character);

	void FlickerIndicator();

	float ElapsedTime;

	UPROPERTY(meta = (BindWidget))
	class UImage* C4IndicatorImage;
};
