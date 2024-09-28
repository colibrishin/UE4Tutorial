// Fill out your copyright notice in the Description page of Project Settings.


#include "WG_Time.h"

#include "Components/TextBlock.h"

#include "MyProject/Jump/Frameworks/GS_Jump.h"

UWG_Time::UWG_Time(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	ElapsedTime = CreateDefaultSubobject<UTextBlock>(TEXT("ElapsedTime"));
	WinText = CreateDefaultSubobject<UTextBlock>(TEXT("WinText"));
}

void UWG_Time::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (AGS_Jump* GameState = GetWorld()->GetGameState<AGS_Jump>())
	{
		GameState->OnCoinGained.AddUniqueDynamic(
			this, &UWG_Time::ShowWinText);
		GameState->OnGameStarted.AddUniqueDynamic(
			this, &UWG_Time::StartTimer);
	}
}

void UWG_Time::NativeDestruct()
{
	Super::NativeDestruct();

	if (AGS_Jump* GameState = GetWorld()->GetGameState<AGS_Jump>())
	{
		GameState->OnCoinGained.RemoveAll(this);
		GameState->OnGameStarted.RemoveAll(this);
	}

	if (UpdateHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(UpdateHandle);
	}
}

void UWG_Time::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UWG_Time::ShowWinText(UChildActorComponent*)
{
	WinText->SetText(FText::FromString("Win!"));
	WinText->SetVisibility(ESlateVisibility::Visible);
}

void UWG_Time::StartTimer(const bool InValue)
{
	// Initial time update;
	UpdateTime();
	
	if (InValue)
	{
		GetWorld()->GetTimerManager().SetTimer(
		UpdateHandle,
		this,
		&UWG_Time::UpdateTime,
		1.f,
		true);
	}
	else
	{
		if (UpdateHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(UpdateHandle);
		}
	}
}

void UWG_Time::UpdateTime()
{
	if (const AGS_Jump* GameState = GetWorld()->GetGameState<AGS_Jump>())
	{
		const double TotalSeconds =
			GameState->IsStarted() ?
				GetWorld()->GetTimeSeconds() - GameState->GetStartTime() :
				GameState->GetEndTime() - GameState->GetStartTime();
		
		const int32 Seconds = FMath::Fmod(TotalSeconds, 60);
		const int32 Minutes = TotalSeconds / 60;
		const int32 Hours = TotalSeconds / 3600;
		const FString TimeFormat = FString::Printf(TEXT("%02d:%02d:%02d"), Hours, Minutes, Seconds);
	
		ElapsedTime->SetText(FText::FromString(TimeFormat));
	}
}


