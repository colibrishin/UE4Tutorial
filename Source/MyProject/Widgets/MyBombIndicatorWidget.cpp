// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBombIndicatorWidget.h"

#include "MyProject/MyC4.h"
#include "MyProject/MyGameState.h"
#include "MyProject/Actors/A_C4.h"
#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Private/Enum.h"

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
		GameState->OnBombStateChanged.AddUniqueDynamic(this, &UMyBombIndicatorWidget::HandleBombStateChanged);
		GameState->OnBombPicked.AddUniqueDynamic(this, &UMyBombIndicatorWidget::OnBombPicked);
	}

	ElapsedTime = 0.f;
}

void UMyBombIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry , InDeltaTime);

	if (const auto& GameState = GetPlayerContext().GetGameState<AMyGameState>())
	{
		if (const AA_C4* RoundC4 = GameState->GetC4())
		{
			const EMyBombState BombState = RoundC4->GetBombState();
			
			if (BombState == EMyBombState::Planted || BombState == EMyBombState::Defusing)
			{
				const float& BombElapsed = GameState->GetC4()->GetAfterPlantElapsedTime();
				const float& BombTime = GameState->GetC4()->GetDetonationTime();

				if (BombElapsed < BombTime - 10)
				{
					ElapsedTime += InDeltaTime;

					if (ElapsedTime > 1.f)
					{
						FlickerIndicator();
						ElapsedTime = 0.f;
					}
				}
				else if (BombElapsed < BombTime - 5)
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
	
}

void UMyBombIndicatorWidget::HandleBombStateChanged(
	const EMyBombState /*InOldState*/, const EMyBombState InNewState, const AA_Character* /*InPlanter*/, const AA_Character* /*InDefuser*/
)
{
	if (InNewState == EMyBombState::Planting)
	{
		return;
	}

	switch (InNewState)
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

void UMyBombIndicatorWidget::OnBombPicked(AA_Character* InCharacter)
{
	if (IsValid(InCharacter))
	{
		if (InCharacter == GetPlayerContext().GetPlayerController()->GetCharacter())
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
