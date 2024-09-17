// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "MyBombIndicatorWidget.generated.h"

enum class EMyBombState : uint8;
class AA_Character;
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
	UFUNCTION()
	void HandleBombStateChanged(
		const EMyBombState InOldState , const EMyBombState InNewState , const AA_Character* InPlanter ,
		const AA_Character*
		InDefuser
	);

	UFUNCTION()
	void OnBombPicked(AA_Character* InCharacter);

	void FlickerIndicator();

	float ElapsedTime;

	UPROPERTY(meta = (BindWidget))
	class UImage* C4IndicatorImage;
};
