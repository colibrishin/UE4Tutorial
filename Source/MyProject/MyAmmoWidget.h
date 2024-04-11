// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	void UpdateAmmo(const int32 AmmoCount, const int32 ClipSize, const int32 ClipCount) const;

private:
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* AmmoText;
};
