// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Private/Utilities.hpp"

#include "Blueprint/UserWidget.h"
#include "MyBuyMenuWeaponWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemCliekd, int32)

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
	void SetID(const int32 ID);

	DECL_BINDON(OnItemClicked, int32);

	int32 GetID() const { return WeaponID; }

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

	int32 WeaponID;

	int32 WeaponPriceValue;

	FOnItemCliekd OnItemClicked;

};
