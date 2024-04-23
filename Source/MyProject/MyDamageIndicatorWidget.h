// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "MyDamageIndicatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyDamageIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindPlayerState(class AMyPlayerState* MyPlayerState);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	bool bVisible = false;

	float DeltaTime = 0.f;

	void HandleDamageTaken(class AMyPlayerState* DamageGiver);

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* IndicatorImage;
};
