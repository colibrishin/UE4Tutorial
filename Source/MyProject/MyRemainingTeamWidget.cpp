// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyRemainingTeamWidget.h"

#include "MyGameState.h"

#include "Components/TextBlock.h"

void UMyRemainingTeamWidget::BindGameState(AMyGameState* State) const
{
	if (IsValid(State))
	{
		LOG_FUNC(LogTemp, Warning, "Bind game state");
		State->BindOnAliveCountChanged(this, &UMyRemainingTeamWidget::HandlePlayerChanges);
	}
}

void UMyRemainingTeamWidget::HandlePlayerChanges(const EMyTeam Team, const int32 Count) const
{
	LOG_FUNC(LogTemp, Warning, "Alive count change received");

	if (const auto& GameState = GetWorld()->GetGameState<AMyGameState>())
	{
		LOG_FUNC(LogTemp, Warning, "Update player counts");

		const int32 AliveCT = GameState->GetCTCount();
		const int32 AliveT = GameState->GetTCount();

		RemainingPlayersText->SetText(FText::FromString(FString::Printf(TEXT("%d vs %d"), AliveCT, AliveT)));
	}
}
