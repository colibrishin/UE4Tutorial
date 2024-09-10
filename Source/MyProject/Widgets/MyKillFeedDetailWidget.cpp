// Fill out your copyright notice in the Description page of Project Settings.


#include "MyKillFeedDetailWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

UMyKillFeedDetailWidget::UMyKillFeedDetailWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	KillerText = CreateDefaultSubobject<UTextBlock>(TEXT("KillerText"));
	VictimText = CreateDefaultSubobject<UTextBlock>(TEXT("VictimText"));
	WeaponImage = CreateDefaultSubobject<UImage>(TEXT("WeaponImage"));
}

void UMyKillFeedDetailWidget::SetKillerText(const FText& Text) const
{
	KillerText->SetText(Text);
}

void UMyKillFeedDetailWidget::SetVictimText(const FText& Text) const
{
	VictimText->SetText(Text);
}

void UMyKillFeedDetailWidget::SetWeaponImage(UTexture2D* Texture) const
{
	WeaponImage->SetBrushFromTexture(Texture);
}
