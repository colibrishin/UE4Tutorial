// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyBuyMenuWeaponWidget.h"

#include "MyGameInstance.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UMyBuyMenuWeaponWidget::SetImage(UTexture2D* Texture) const
{
	if (IsValid(Texture))
	{
		WeaponImage->SetBrushFromTexture(Texture);
	}
}

void UMyBuyMenuWeaponWidget::SetName(const FString& Name) const
{
	WeaponName->SetText(FText::FromString(Name));
}

void UMyBuyMenuWeaponWidget::SetPrice(const int32 Price)
{
	WeaponPrice->SetText(FText::FromString("$" + FString::FromInt(Price)));
	WeaponPriceValue = Price;
}

void UMyBuyMenuWeaponWidget::SetID(const int32 ID)
{
	WeaponID = ID;
}

FReply UMyBuyMenuWeaponWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnItemClicked.Broadcast(WeaponID);

	return Super::NativeOnMouseButtonDown(InGeometry , InMouseEvent);
}
