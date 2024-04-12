// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "MyBuyMenuWeaponWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyBuyMenuWeaponWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetImage(UTexture2D* Texture) const;
	void SetName(const FString& Name) const;
	void SetPrice(const int32 Price) const;

private:
	UPROPERTY(Meta=(BindWidget))
	class UVerticalBox* WeaponWidgetContainer;

	UPROPERTY(Meta=(BindWidget))
	class UImage* WeaponImage;

	UPROPERTY(Meta=(BindWidget))
	class UTextBlock* WeaponName;

	UPROPERTY(Meta=(BindWidget))
	class UTextBlock* WeaponPrice;

};
