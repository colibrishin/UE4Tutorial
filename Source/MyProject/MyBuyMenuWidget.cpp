// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyBuyMenuWidget.h"

#include "MyBuyMenuWeaponWidget.h"
#include "MyGameInstance.h"
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
			WeaponGridPanel->AddChildToUniformGrid(Widget, i / 4, i % 4);
		}
	}
}

void UMyBuyMenuWidget::Open() const
{
	const auto& Controller = GetOwningLocalPlayer()->PlayerController;

	Controller->SetInputMode(FInputModeUIOnly());
	Controller->SetShowMouseCursor(true);


	
}
