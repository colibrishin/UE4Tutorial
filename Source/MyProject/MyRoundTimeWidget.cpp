// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyRoundTimeWidget.h"

#include "MyGameState.h"
#include "MyProjectGameModeBase.h"

#include "Components/TextBlock.h"

#include "GameFramework/GameStateBase.h"

void UMyRoundTimeWidget::UpdateTime() const
{
	const auto& MyGameState = GetWorld()->GetGameState<AMyGameState>();

	const float Time = MyGameState->GetRemainingRoundTime();

	const int32 Minutes = FMath::FloorToInt(Time / 60);
	const int32 Seconds = FMath::FloorToInt(FMath::Fmod(Time, 60));
	const FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

	RoundTimeText->SetText(FText::FromString(TimeString));
}

void UMyRoundTimeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateTime();
}

void UMyRoundTimeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (const auto& GameState = GetPlayerContext().GetGameState<AMyGameState>())
	{
		GameState->BindOnBombProgressChanged(this, &UMyRoundTimeWidget::HandleBombProgressChanged);
	}
}

void UMyRoundTimeWidget::HandleBombProgressChanged(const EMyBombState State) const
{
	if (State == EMyBombState::Planted || 
		State == EMyBombState::Defusing || 
		State == EMyBombState::Exploded ||
		State == EMyBombState::Defused)
	{
		RoundTimeText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		RoundTimeText->SetVisibility(ESlateVisibility::Visible);
	}
}
