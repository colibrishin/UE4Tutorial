// Fill out your copyright notice in the Description page of Project Settings.


#include "MyKillFeedWidget.h"

#include "MyKillFeedDetailWidget.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "MyProject/MyGameState.h"
#include "MyProject/MyPlayerState.h"
#include "MyProject/Components/Asset/C_WeaponAsset.h"
#include "MyProject/DataAsset/DA_Weapon.h"
#include "MyProject/MyPlayerController.h"

void UMyKillFeedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (const auto& GameState = GetPlayerContext().GetGameState<AMyGameState>())
	{
		GameState->OnKillOccurred.AddUniqueDynamic(this, &UMyKillFeedWidget::HandleKillOccurred);
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
	AMyPlayerController* Killer, AMyPlayerController* Victim, UC_PickUp* Weapon
)
{
	if (Killer && Victim && Weapon)
	{
		AMyPlayerState* KillerPlayerState = Killer->GetPlayerState<AMyPlayerState>();
		AMyPlayerState* VictimPlayerState = Victim->GetPlayerState<AMyPlayerState>();

		const auto& KillerName = FText::FromString(*KillerPlayerState->GetPlayerName());
		const auto& VictimName = FText::FromString(*VictimPlayerState->GetPlayerName());
		UTexture2D* WeaponImage = nullptr;

		if (const UC_WeaponAsset* AssetComponent = Weapon->GetOwner()->GetComponentByClass<UC_WeaponAsset>())
		{
			const UDA_Weapon* WeaponAsset = AssetComponent->GetAsset<UDA_Weapon>();
			ensure(WeaponAsset);

			WeaponImage = WeaponAsset->GetImage();
		}

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
