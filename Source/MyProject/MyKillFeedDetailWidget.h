// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyKillFeedDetailWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyKillFeedDetailWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMyKillFeedDetailWidget(const FObjectInitializer& ObjectInitializer);

	void SetKillerText(const FText& Text) const;
	void SetVictimText(const FText& Text) const;
	void SetWeaponImage(UTexture2D* Texture) const;

private:
	UPROPERTY(Meta=(BindWidget))
	class UTextBlock* KillerText;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* VictimText;

	UPROPERTY(meta=(BindWidget))
	class UImage* WeaponImage;
	
};
