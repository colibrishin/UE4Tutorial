// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBombProgressWidget.h"

#include "../MyC4.h"
#include "../MyGameState.h"
#include "MyProject/Actors/BaseClass/A_Character.h"

#include "Components/ProgressBar.h"

#include "MyProject/Actors/A_C4.h"

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
		GameState->OnBombStateChanged.AddUniqueDynamic(this, &UMyBombProgressWidget::OnBombStateChanged);
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
				if (Bomb->GetBombState() == EMyBombState::Planting)
				{
					SetValue(Bomb->GetElapsedPlantTimeRatio());
				}
				else if (Bomb->GetBombState() == EMyBombState::Defusing)
				{
					SetValue(Bomb->GetElapsedDefuseTimeRatio());
				}
			}
		}
	}
}

void UMyBombProgressWidget::OnBombStateChanged(const EMyBombState InOldState, const EMyBombState InNewState, const AA_Character* InPlanter, const AA_Character* InDefuser)
{
	LOG_FUNC_PRINTF(LogTemp, Warning, "Bomb state changed to: %s", *EnumToString(InNewState));

	if (InNewState == EMyBombState::Planting || InNewState == EMyBombState::Defusing)
	{
		if (InNewState == EMyBombState::Planting)
		{
			if (InPlanter == GetPlayerContext().GetPlayerController()->GetCharacter())
			{
				bNeedToShow = true;
			}
		}
		else if (InNewState == EMyBombState::Defusing)
		{
			if (InDefuser == GetPlayerContext().GetPlayerController()->GetCharacter())
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
