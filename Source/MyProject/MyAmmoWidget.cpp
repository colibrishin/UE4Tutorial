// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyAmmoWidget.h"

#include "Components/TextBlock.h"

void UMyAmmoWidget::UpdateAmmo(const int32 CurrentAmmoCount, const int32 RemainingAmmoCount) const
{
	const FText Formatted = FText::Format(FText::FromString(TEXT("{0}/{1}")), 
	                                     FText::AsNumber(CurrentAmmoCount), 
	                                     FText::AsNumber(RemainingAmmoCount));

	AmmoText->SetText(Formatted);
}
