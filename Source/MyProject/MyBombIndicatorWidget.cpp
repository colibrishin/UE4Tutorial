// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyBombIndicatorWidget.h"

#include "MyC4.h"
#include "MyCharacter.h"
#include "MyGameState.h"
#include "MyPlayerState.h"

void UMyBombIndicatorWidget::Reset()
{
	SetRenderOpacity(0.f);
	ElapsedTime = 0.f;
}

void UMyBombIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetRenderOpacity(0.f);

	if (const auto& GameState = GetPlayerContext().GetGameState<AMyGameState>())
	{
		GameState->BindOnBombProgressChanged(this, &UMyBombIndicatorWidget::OnBombProgressChanged);
		GameState->BindOnBombPicked(this, &UMyBombIndicatorWidget::OnBombPicked);
	}

	ElapsedTime = 0.f;
}

void UMyBombIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry , InDeltaTime);

	if (const auto& GameState = GetPlayerContext().GetGameState<AMyGameState>())
	{
		const auto& BombState = GameState->GetBombState();

		if (BombState == EMyBombState::Planted || BombState == EMyBombState::Defusing)
		{
			const auto& BombElapsed = GameState->GetC4()->GetElapsed();

			if (BombElapsed < AMyC4::FullExplodingTime - 10)
			{
				ElapsedTime += InDeltaTime;

				if (ElapsedTime > 1.f)
				{
					FlickerIndicator();
					ElapsedTime = 0.f;
				}
			}
			else if (BombElapsed < AMyC4::FullExplodingTime - 5)
			{
				ElapsedTime += InDeltaTime;

				if (ElapsedTime > 0.5f)
				{
					FlickerIndicator();
					ElapsedTime = 0.f;
				}
			}
			else
			{
				ElapsedTime += InDeltaTime;

				if (ElapsedTime > 0.2f)
				{
					FlickerIndicator();
					ElapsedTime = 0.f;
				}
			}
		}
	}
	
}

void UMyBombIndicatorWidget::OnBombProgressChanged(const EMyBombState State)
{
	if (State == EMyBombState::Planting)
	{
		return;
	}

	switch (State)
	{
	case EMyBombState::Planted:
		ElapsedTime = 0.f;
		SetRenderOpacity(1.0f);
		break;
	case EMyBombState::Defusing:
		SetRenderOpacity(1.0f);
		break;
	case EMyBombState::Defused:
	case EMyBombState::Exploded:
		SetRenderOpacity(0.2f);
		break;
	default:
		SetRenderOpacity(0.f);
		break;
	}
}

void UMyBombIndicatorWidget::OnBombPicked(AMyCharacter* Character)
{
	if (IsValid(Character))
	{
		if (Character == GetPlayerContext().GetPlayerController()->GetCharacter())
		{
			SetRenderOpacity(1.0f);
		}
		else
		{
			SetRenderOpacity(0.f);
		}
	}
}

void UMyBombIndicatorWidget::FlickerIndicator()
{
	if (GetRenderOpacity() == 0.5f)
	{
		SetRenderOpacity(1.0f);
	}
	else
	{
		SetRenderOpacity(0.5f);
	}
}
