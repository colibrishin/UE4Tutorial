// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyBuyMenuWidget.h"

#include "ConstantFVector.hpp"
#include "Data.h"
#include "MyBuyMenuWeaponWidget.h"
#include "MyCharacter.h"
#include "MyGameInstance.h"
#include "MyInGameHUD.h"
#include "MyPlayerController.h"
#include "MyStatComponent.h"
#include "MyWeapon.h"
#include "MyWeaponDataAsset.h"
#include "Utilities.hpp"
#include "GameFramework/PlayerController.h"
#include "Runtime/Engine/Classes/Engine/Player.h"
#include "Components/GridPanel.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "Components/UniformGridPanel.h"

#include "Kismet/GameplayStatics.h"

void UMyBuyMenuWidget::Populate()
{
	const auto& Instance = Cast<UMyGameInstance>(GetGameInstance());

	if (IsValid(Instance))
	{
		for (int i = 1; i < Instance->GetWeaponCount(); ++i)
		{
			const auto& WeaponData = GetWeaponData(this, i);

			if (WeaponData == nullptr)
			{
				continue;
			}

			const auto& WeaponStat = WeaponData->WeaponDataAsset->GetWeaponStat();

			const auto  Name      = FName(*FString::Printf(TEXT("WeaponMenu%d"), i));
			const auto  RawWidget = CreateWidget(GetRootWidget(), ItemWidgetClass);
			const auto  Widget    = Cast<UMyBuyMenuWeaponWidget>(RawWidget);

			Widget->SetName(WeaponStat.Name);
			Widget->SetPrice(WeaponStat.Price);
			Widget->SetID(i);
			Widget->BindOnItemClicked(this, &UMyBuyMenuWidget::ProcessBuy);
			WeaponGridPanel->AddChildToUniformGrid(Widget, i / 4, i % 4);
		}
	}
}

void UMyBuyMenuWidget::Open()
{
	if (IsOpen)
	{
		LOG_FUNC(LogTemp, Error, "Already opened");
		return;
	}

	if (const auto BuyTime = Cast<AMyProjectGameModeBase>(UGameplayStatics::GetGameMode(this))->HasBuyTimeEnded())
	{
		LOG_FUNC(LogTemp, Error, "Cannot open buy menu after buy time is over");
		return;
	}

	LOG_FUNC(LogTemp, Warning, "Opening");

	if (!IsInViewport())
	{
		AddToViewport();
	}

	const auto& Controller = GetOwningLocalPlayer()->PlayerController;
	const auto& Character = Cast<AMyCharacter>(Controller->GetPawn());

	if (IsValid(Character))
	{
		UpdateMoney(Character->GetStatComponent()->GetMoney());
	}

	if (Controller->IsInState("Dead"))
	{
		return;
	}

	Controller->SetShowMouseCursor(true);
	const FInputModeGameAndUI InputModeData;
	Controller->SetInputMode(InputModeData);

	SetVisibility(ESlateVisibility::Visible);
	IsOpen = true;
}

void UMyBuyMenuWidget::Close()
{
	if (!IsOpen)
	{
		LOG_FUNC(LogTemp, Error, "Already closed");
		return;
	}

	LOG_FUNC(LogTemp, Warning, "Closing");

	if (IsInViewport())
	{
		RemoveFromViewport();
	}

	const auto& Controller = GetOwningLocalPlayer()->PlayerController;

	Controller->SetShowMouseCursor(false);
	const FInputModeGameOnly InputModeData;
	Controller->SetInputMode(InputModeData);

	SetVisibility(ESlateVisibility::Hidden);
	IsOpen = false;
}

void UMyBuyMenuWidget::Toggle()
{
	if (!IsOpen)
	{
		Open();
	}
	else
	{
		Close();
	}
}

bool UMyBuyMenuWidget::Validate(const int32 ID, AMyCharacter* const& Character) const
{
	const auto& Instance   = Cast<UMyGameInstance>(GetGameInstance());
	const auto& WeaponData = GetWeaponData(this, ID);
	const auto& WeaponStat = WeaponData->WeaponDataAsset->GetWeaponStat();

	if (WeaponData == nullptr)
	{
		LOG_FUNC(LogTemp, Error, "WeaponInfo is nullptr");
		return false;
	}

	if (Character->GetStatComponent()->GetMoney() <= 0)
	{
		LOG_FUNC(LogTemp, Error, "Player => Not enough money");
		return false;
	}

	if (Character->GetStatComponent()->GetMoney() - WeaponStat.Price < 0)
	{
		LOG_FUNC(LogTemp, Error, "Player has money but have not enough money to buy");
		return false;
	}

	return true;
}

void UMyBuyMenuWidget::BindPlayer(AMyCharacter* Character)
{
	Character->GetStatComponent()->BindOnMoneyChanged(this, &UMyBuyMenuWidget::UpdateMoney);
}

void UMyBuyMenuWidget::ProcessBuy(const int32 ID) const
{
	if (GetVisibility() != ESlateVisibility::Visible)
	{
		return;
	}

	const auto& Controller =  Cast<AMyPlayerController>(GetOwningLocalPlayer()->PlayerController);
	const auto& Character = Cast<AMyCharacter>(Controller->GetPawn());

	if (IsValid(Character))
	{
		if (!Validate(ID, Character)) 
		{
			return;
		}

		Controller->BuyWeapon(ID);
	}

}

void UMyBuyMenuWidget::UpdateMoney(const int32 Money) const
{
	CurrentMoney->SetText(FText::FromString(FString::Printf(TEXT("Money: %d"), Money)));
}
