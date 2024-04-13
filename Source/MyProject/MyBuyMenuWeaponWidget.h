// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utilities.hpp"

#include "Blueprint/UserWidget.h"
#include "MyBuyMenuWeaponWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemCliekd, float)

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
	void SetPrice(const int32 Price);

	DECL_BINDON(OnItemClicked, float);

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY(Meta=(BindWidget))
	class UVerticalBox* WeaponWidgetContainer;

	UPROPERTY(Meta=(BindWidget))
	class UImage* WeaponImage;

	UPROPERTY(Meta=(BindWidget))
	class UTextBlock* WeaponName;

	UPROPERTY(Meta=(BindWidget))
	class UTextBlock* WeaponPrice;

	int32 WeaponPriceValue;

	FOnItemCliekd OnItemClicked;

};
