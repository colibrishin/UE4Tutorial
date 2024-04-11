// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyAmmoWidget.h"

#include "Components/TextBlock.h"

void UMyAmmoWidget::UpdateAmmo(const int32 AmmoCount, const int32 ClipSize, const int32 ClipCount) const
{
	const FText Formatted = FText::Format(FText::FromString(TEXT("{0}/{1}")), 
	                                     FText::AsNumber(AmmoCount), 
	                                     FText::AsNumber(ClipCount * ClipSize));

	AmmoText->SetText(Formatted);
}
