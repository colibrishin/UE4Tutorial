// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyInGameStatEntryWidget.h"

#include "MyPlayerState.h"

#include "Components/TextBlock.h"

void UMyInGameStatEntryWidget::SetPlayer(AMyPlayerState* PlayerState)
{
	PlayerName->SetText(FText::FromString(PlayerState->GetPlayerName()));
	DesignatedPlayerState = PlayerState;

	if (IsValid(PlayerState))
	{
		KillCount->SetText(FText::FromString(FString::FromInt(DesignatedPlayerState->GetKill())));
		DeathCount->SetText(FText::FromString(FString::FromInt(DesignatedPlayerState->GetDeath())));
		AssistCount->SetText(FText::FromString(FString::FromInt(DesignatedPlayerState->GetAssist())));
		PingValue->SetText(FText::FromString(FString::FromInt(DesignatedPlayerState->GetPing())));
	}
}

void UMyInGameStatEntryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	DeltaTime += InDeltaTime;

	if (DeltaTime > 1.f)
	{
		if (!DesignatedPlayerState.IsValid())
		{
			Destruct();
		}

		ForceUpdatePlayerStats();
		DeltaTime = 0.f;
	}
}

void UMyInGameStatEntryWidget::ForceUpdatePlayerStats() const
{
	if (DesignatedPlayerState.IsValid())
	{
		KillCount->SetText(FText::FromString(FString::FromInt(DesignatedPlayerState->GetKill())));
		DeathCount->SetText(FText::FromString(FString::FromInt(DesignatedPlayerState->GetDeath())));
		AssistCount->SetText(FText::FromString(FString::FromInt(DesignatedPlayerState->GetAssist())));
		PingValue->SetText(FText::FromString(FString::FromInt(DesignatedPlayerState->GetPing())));
	}
}

void UMyInGameStatEntryWidget::HandleStateChanges(AMyPlayerController* Controller, EMyTeam Team, EMyCharacterState State) const
{
	ForceUpdatePlayerStats();
}
