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

	const auto& CheckLocalPlayer = [this]( const AA_Character* Character )
		{
			return Character && Character->GetController() == GetPlayerContext().GetPlayerController();
		};

	switch (InNewState)
	{
	case EMyBombState::Defusing:
		bNeedToShow = CheckLocalPlayer(InDefuser);
		break;
	case EMyBombState::Planting:
		bNeedToShow = CheckLocalPlayer(InPlanter);
		break;
	default:
		bNeedToShow = false;
	}

	// todo: Show regardless if player is spectator
	if (bNeedToShow)
	{
		SetRenderOpacity( 1.f );
	}
	else
	{
		SetRenderOpacity( 0.f );
	}
}
