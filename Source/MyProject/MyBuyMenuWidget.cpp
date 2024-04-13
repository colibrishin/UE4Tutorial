// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyBuyMenuWidget.h"

#include "MyBuyMenuWeaponWidget.h"
#include "MyCharacter.h"
#include "MyGameInstance.h"
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
	AddToViewport();
	const auto& Controller = GetOwningLocalPlayer()->PlayerController;

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
	RemoveFromViewport();
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

void UMyBuyMenuWidget::ProcessBuy(const float Price) const
{
	if (GetVisibility() != ESlateVisibility::Visible)
	{
		return;
	}

	if (Price > 0)
	{
		const auto& Character = Cast<AMyCharacter>(GetOwningLocalPlayer()->PlayerController->GetPawn());

		if (IsValid(Character))
		{
			if (Character->GetStatComponent()->GetMoney() - Price <= 0)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not enough money!"));
				return;
			}

			Server_RequestBuy(Character, Price);
		}
	}

}

void UMyBuyMenuWidget::Server_RequestBuy_Implementation(AMyCharacter* Character, const float Price) const
{
	if (IsValid(Character))
	{
		// Second chance check.
		if (Character->GetStatComponent()->GetMoney() - Price <= 0)
		{
			return;
		}

		Character->GetStatComponent()->AddMoney(-Price);
	}
}
