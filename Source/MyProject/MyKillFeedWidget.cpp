// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyKillFeedWidget.h"

#include "MyGameState.h"
#include "MyKillFeedDetailWidget.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "MyWeapon.h"

#include "Components/GridPanel.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"

void UMyKillFeedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (const auto& GameState = GetPlayerContext().GetGameState<AMyGameState>())
	{
		GameState->BindOnKillOccurred(this, &UMyKillFeedWidget::HandleKillOccurred);
	}
}

void UMyKillFeedWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry , InDeltaTime);

	if (!Empty())
	{
		DeltaTime += InDeltaTime;
	}

	if (DeltaTime >= KillFeedDuration)
	{
		PopKillFeed();
		DeltaTime = 0.0f;
	}
}

void UMyKillFeedWidget::HandleKillOccurred(
	AMyPlayerState* Killer, AMyPlayerState* Victim, const AMyWeapon* Weapon
)
{
	if (Killer && Victim && Weapon)
	{
		const auto& KillerName = FText::FromString(*Killer->GetPlayerName());
		const auto& VictimName = FText::FromString(*Victim->GetPlayerName());
		const auto& WeaponImage = Weapon->GetWeaponImage();

		AddKillFeed(KillerName, VictimName, WeaponImage);
	}
}

void UMyKillFeedWidget::AddKillFeed(const FText& KillerName, const FText& VictimName, UTexture2D* WeaponImage)
{
	const auto& KillFeedEntry = Cast<UMyKillFeedDetailWidget>(CreateWidget(GetRootWidget(), EntryClass));

	KillFeedEntry->SetKillerText(KillerName);
	KillFeedEntry->SetVictimText(VictimName);
	KillFeedEntry->SetWeaponImage(WeaponImage);

	PushKillFeed(KillFeedEntry);
}

void UMyKillFeedWidget::PushKillFeed(UMyKillFeedDetailWidget* Widget)
{
	if (KillFeedPanel)
	{
		KillFeedPanel->AddChildToVerticalBox(Widget);
	}
}

void UMyKillFeedWidget::PopKillFeed() const
{
	if (KillFeedPanel)
	{
		if (KillFeedPanel->GetChildrenCount() > 0)
		{
			KillFeedPanel->RemoveChildAt(0);
		}
	}
}

bool UMyKillFeedWidget::Empty() const
{
	return KillFeedPanel->GetChildrenCount() == 0;
}
