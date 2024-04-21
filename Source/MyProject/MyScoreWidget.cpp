// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyScoreWidget.h"

#include "Components/TextBlock.h"

void UMyScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (const auto& GameState = GetPlayerContext().GetGameState<AMyGameState>())
	{
		BindGameState(GameState);
	}
}

void UMyScoreWidget::BindGameState(AMyGameState* State)
{
	if (IsValid(State))
	{
		State->BindOnWinnerSet(this, &UMyScoreWidget::UpdateScore);
	}
}

void UMyScoreWidget::UpdateScore(const EMyTeam Team) const
{
	if (IsValid(ScoreText))
	{
		const auto& CTWins = GetWorld()->GetGameState<AMyGameState>()->GetCTWins();
		const auto& TWins = GetWorld()->GetGameState<AMyGameState>()->GetTWins();

		ScoreText->SetText(FText::FromString(FString::Printf(TEXT("CT: %d T: %d"), CTWins, TWins)));
	}
}
