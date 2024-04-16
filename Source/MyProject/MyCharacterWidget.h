// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPlayerState.h"

#include "Blueprint/UserWidget.h"
#include "MyCharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindHp(AMyPlayerState* State);

private:
	void UpdateHpRatio(const int32 PlayerId, const float Value) const;

	UPROPERTY(Meta=(BindWidget))
	class UProgressBar* HPProgressBar;
};
