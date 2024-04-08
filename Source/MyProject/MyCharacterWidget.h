// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	void BindHp(class UMyStatComponent* Component);

private:
	void UpdateHpRatio(float Value) const;

	UPROPERTY(Meta=(BindWidget))
	class UProgressBar* HPProgressBar;
};
