// Fill out your copyright notice in the Description page of Project Settings.


#include "MyRoundTimeWidget.h"

#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "MyProject/MyGameState.h"

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
		GameState->OnBombStateChanged.AddUniqueDynamic(this, &UMyRoundTimeWidget::HandleBombProgressChanged);
	}
}

void UMyRoundTimeWidget::HandleBombProgressChanged(const EMyBombState /*InOldState*/, const EMyBombState InNewState, const AA_Character* /*InPlanter*/, const AA_Character* /*InDefuser*/)
{
	if (InNewState == EMyBombState::Planted || 
		InNewState == EMyBombState::Defusing || 
		InNewState == EMyBombState::Exploded ||
		InNewState == EMyBombState::Defused)
	{
		RoundTimeText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		RoundTimeText->SetVisibility(ESlateVisibility::Visible);
	}
}
