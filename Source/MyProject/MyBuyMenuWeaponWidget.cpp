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

void UMyBuyMenuWeaponWidget::SetPrice(const int32 Price) const
{
	WeaponPrice->SetText(FText::FromString("$" + FString::FromInt(Price)));
}
