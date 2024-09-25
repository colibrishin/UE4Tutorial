// Fill out your copyright notice in the Description page of Project Settings.


#include "WG_Time.h"

#include "Components/TextBlock.h"

UWG_Time::UWG_Time(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	ElapsedTime = CreateDefaultSubobject<UTextBlock>(TEXT("ElapsedTime"));
}

void UWG_Time::NativeConstruct()
{
	Super::NativeConstruct();

	GetWorld()->GetTimerManager().SetTimer(
		UpdateHandle,
		this,
		&UWG_Time::UpdateTime,
		1.f,
		true);
}

void UWG_Time::UpdateTime()
{
	const double TotalSeconds = GetWorld()->GetTimeSeconds();

	const int32 Seconds = FMath::Fmod(TotalSeconds, 60);
	const int32 Minutes = TotalSeconds / 60;
	const int32 Hours = TotalSeconds / 3600;
	const FString TimeFormat = FString::Printf(TEXT("%02d:%02d:%02d"), Hours, Minutes, Seconds);
	
	ElapsedTime->SetText(FText::FromString(TimeFormat));
}


