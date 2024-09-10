// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBombProgressWidget.h"

#include "../MyC4.h"
#include "../MyCharacter.h"
#include "../MyGameState.h"

#include "Components/ProgressBar.h"

void UMyBombProgressWidget::SetValue(const float Value) const
{
	ProgressBar->SetPercent(Value);
}

void UMyBombProgressWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (const auto& GameState = GetPlayerContext().GetGameState<AMyGameState>())
	{
		BindGameState(GameState);
	}
}

void UMyBombProgressWidget::BindGameState(AMyGameState* GameState)
{
	if (GameState)
	{
		LOG_FUNC(LogTemp, Warning, "Binding to GameState");
		GameState->BindOnBombProgressChanged(this, &UMyBombProgressWidget::OnBombStateChanged);
	}
}

void UMyBombProgressWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bNeedToShow)
	{
		if (const auto& GameState = GetWorld()->GetGameState<AMyGameState>())
		{
			if (const auto& Bomb = GameState->GetC4())
			{
				if (GameState->GetBombState() == EMyBombState::Planting)
				{
					SetValue(Bomb->GetPlantingRatio());
				}
				else if (GameState->GetBombState() == EMyBombState::Defusing)
				{
					SetValue(Bomb->GetDefusingRatio());
				}
			}
		}
	}
}

void UMyBombProgressWidget::OnBombStateChanged(const EMyBombState NewState)
{
	LOG_FUNC_PRINTF(LogTemp, Warning, "Bomb state changed to: %s", *EnumToString(NewState));

	if (NewState == EMyBombState::Planting || NewState == EMyBombState::Defusing)
	{
		const auto& GameState = GetWorld()->GetGameState<AMyGameState>();

		if (NewState == EMyBombState::Planting)
		{
			if (GameState->GetC4()->GetItemOwner() == Cast<AMyCharacter>(GetPlayerContext().GetPlayerController()->GetCharacter()))
			{
				bNeedToShow = true;
			}
		}
		else if (NewState == EMyBombState::Defusing)
		{
			if (GameState->GetC4()->GetDefusingCharacter() == Cast<AMyCharacter>(GetPlayerContext().GetPlayerController()->GetCharacter()))
			{
				bNeedToShow = true;
			}
		}
		// todo: Show regardless if player is spectator
	}
	else
	{
		bNeedToShow = false;
	}

	if (bNeedToShow)
	{
		SetRenderOpacity(1.f);
		AddToViewport();
	}
	else
	{
		SetRenderOpacity(0.f);
		RemoveFromParent();
	}
}
