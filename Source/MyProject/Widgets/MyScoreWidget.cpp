// Fill out your copyright notice in the Description page of Project Settings.


#include "MyScoreWidget.h"

#include "Components/TextBlock.h"
#include "../MyGameState.h"

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
		State->OnWinnerSet.AddUniqueDynamic(this, &UMyScoreWidget::UpdateScore);
	}
}

void UMyScoreWidget::UpdateScore(const EMyTeam Team)
{
	if (IsValid(ScoreText))
	{
		const auto& CTWins = GetWorld()->GetGameState<AMyGameState>()->GetCTWins();
		const auto& TWins = GetWorld()->GetGameState<AMyGameState>()->GetTWins();

		ScoreText->SetText(FText::FromString(FString::Printf(TEXT("CT: %d T: %d"), CTWins, TWins)));
	}
}
