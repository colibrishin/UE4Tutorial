// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyInGameStatWidget.h"

#include "MyGameState.h"
#include "MyPlayerState.h"

#include "MyInGameStatEntryWidget.h"

#include "Components/VerticalBox.h"

void UMyInGameStatWidget::Open()
{
	AddToViewport();
}

void UMyInGameStatWidget::Close()
{
	RemoveFromViewport();
}

void UMyInGameStatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (const auto& GameState = GetPlayerContext().GetGameState<AMyGameState>())
	{
		for (const auto& Player : GameState->PlayerArray)
		{
			AddNewPlayer(Cast<AMyPlayerState>(Player));
		}
	}
}

void UMyInGameStatWidget::AddNewPlayer(AMyPlayerState* State) const
{
	if (IsValid(State))
	{
		for (const auto& Child : CTStats->GetAllChildren())
		{
			if (const auto& Entry = Cast<UMyInGameStatEntryWidget>(Child))
			{
				if (Entry->GetPlayer() == State)
				{
					return;
				}

				if (Entry->GetPlayer() == nullptr)
				{
					CTStats->RemoveChild(Entry);
				}
			}
		}

		for (const auto& Child : TStats->GetAllChildren())
		{
			if (const auto& Entry = Cast<UMyInGameStatEntryWidget>(Child))
			{
				if (Entry->GetPlayer() == State)
				{
					return;
				}

				if (Entry->GetPlayer() == nullptr)
				{
					TStats->RemoveChild(Entry);
				}
			}
		}

		const auto& NewEntry = Cast<UMyInGameStatEntryWidget>(CreateWidget(GetRootWidget(), EntryClass));

		NewEntry->SetPlayer(State);

		if (State->GetTeam() == EMyTeam::CT)
		{
			CTStats->AddChildToVerticalBox(NewEntry);
		}
		else if (State->GetTeam() == EMyTeam::T)
		{
			TStats->AddChildToVerticalBox(NewEntry);
		}
	}
}
