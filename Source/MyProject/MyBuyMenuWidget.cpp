// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyBuyMenuWidget.h"

#include "MyBuyMenuWeaponWidget.h"
#include "MyCharacter.h"
#include "MyGameInstance.h"
#include "MyInGameHUD.h"
#include "MyStatComponent.h"
#include "Utilities.hpp"
#include "GameFramework/PlayerController.h"
#include "Runtime/Engine/Classes/Engine/Player.h"
#include "Components/GridPanel.h"
#include "Components/TileView.h"
#include "Components/UniformGridPanel.h"

#include "Kismet/GameplayStatics.h"

void UMyBuyMenuWidget::Populate() const
{
	const auto& Instance = Cast<UMyGameInstance>(GetGameInstance());

	if (IsValid(Instance))
	{
		for (int i = 1; i < Instance->GetWeaponCount(); ++i)
		{
			const auto& Weapon = Instance->GetWeaponValue(i);

			if (Weapon == nullptr)
			{
				continue;
			}

			const auto  Name      = FName(*FString::Printf(TEXT("WeaponMenu%d"), i));
			const auto  RawWidget = CreateWidget(GetRootWidget(), ItemWidgetClass);
			const auto  Widget    = Cast<UMyBuyMenuWeaponWidget>(RawWidget);

			Widget->SetName(Weapon->Name);
			Widget->SetPrice(Weapon->Price);
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

	LOG_FUNC(LogTemp, Warning, "Opening");

	if (!IsInViewport())
	{
		AddToViewport();
	}

	const auto& Controller = GetOwningLocalPlayer()->PlayerController;

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

void UMyBuyMenuWidget::ProcessBuy(const int32 ID) const
{
	if (GetVisibility() != ESlateVisibility::Visible)
	{
		return;
	}

	const auto& Character = Cast<AMyCharacter>(GetOwningLocalPlayer()->PlayerController->GetPawn());

	if (IsValid(Character))
	{
		if (!Validate(ID , Character)) 
		{
			return;
		}

		Server_RequestBuy(Character, ID);
	}

}

void UMyBuyMenuWidget::Server_RequestBuy_Implementation(AMyCharacter* Character, const int32 ID) const
{
	if (IsValid(Character))
	{
		// Second chance check.
		const auto& WeaponData = GetWeaponData(this, ID);
		const auto& WeaponStat = WeaponData->WeaponDataAsset->GetWeaponStat();

		if (!Validate(ID, Character))
		{
			return;
		}

		Character->GetStatComponent()->AddMoney(-WeaponStat.Price);


	}
}
