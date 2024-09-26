// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBuyMenuWidget.h"

#include "../Private/CommonBuy.hpp"
#include "MyBuyMenuWeaponWidget.h"
#include "../MyGameState.h"

#include "../MyPlayerController.h"
#include "../MyPlayerState.h"
#include "../Private/Utilities.hpp"
#include "GameFramework/PlayerController.h"
#include "Runtime/Engine/Classes/Engine/Player.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"

#include "Kismet/GameplayStatics.h"
#include "MyProject/Components/C_Buy.h"

void UMyBuyMenuWidget::Populate()
{
	const auto& Subsystem = GetWorld()->GetSubsystem<USS_World>();

	if (IsValid(Subsystem))
	{
		for (int i = 1; i < Subsystem->GetAssetCount(); ++i)
		{
			const auto& WeaponData = Subsystem->GetRowData<FBaseAssetRow>(i);
			
			if (WeaponData == nullptr)
			{
				continue;
			}

			const UDA_Weapon* WeaponAsset = Cast<UDA_Weapon>(WeaponData->AssetToLink);

			if (!WeaponAsset)
			{
				continue;
			}
			
			const auto  Name      = FName(*FString::Printf(TEXT("WeaponMenu%d"), i));
			const auto  RawWidget = CreateWidget(GetRootWidget(), ItemWidgetClass);
			const auto  Widget    = Cast<UMyBuyMenuWeaponWidget>(RawWidget);

			Widget->SetName(WeaponAsset->GetAssetName());
			Widget->SetPrice(WeaponAsset->GetPrice());
			Widget->SetID(WeaponAsset->GetID());
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

	const auto BuyTime = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));
	const auto& Controller = GetOwningLocalPlayer()->PlayerController;
	const auto& Character = Cast<AA_Character>(Controller->GetPawn());

	if (!BuyTime->CanBuy())
	{
		LOG_FUNC(LogTemp, Error, "Cannot open buy menu after buy time is over");
		return;
	}

	if (!IsPlayerInBuyZone(Character))
	{
		return;
	}

	LOG_FUNC(LogTemp, Warning, "Opening");

	if (!IsInViewport())
	{
		AddToViewport();
	}

	if (IsValid(Character))
	{
		UpdateMoney(Character->GetPlayerState<AMyPlayerState>()->GetMoney());
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
		RemoveFromParent();
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

void UMyBuyMenuWidget::DispatchPlayerState(AMyPlayerState* InPlayerState)
{
	check(InPlayerState);
	InPlayerState->OnMoneyChanged.AddUniqueDynamic(this, &UMyBuyMenuWidget::UpdateMoney);
}

void UMyBuyMenuWidget::BuyTimeEnded(bool NewBuyTime)
{
	if (!NewBuyTime)
	{
		LOG_FUNC(LogTemp, Warning, "Buy time ended");
		Close();
	}
}

void UMyBuyMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (const auto& Controller = Cast<AMyPlayerController>(GetOwningLocalPlayer()->PlayerController))
	{
		DispatchPlayerState(Controller->GetPlayerState<AMyPlayerState>());
	}
}

void UMyBuyMenuWidget::ProcessBuy(const int32 ID) const
{
	if (GetVisibility() != ESlateVisibility::Visible)
	{
		return;
	}

	const auto& Controller =  Cast<AMyPlayerController>(GetOwningLocalPlayer()->PlayerController);
	const auto& Character = Cast<AA_Character>(Controller->GetPawn());

	if (IsValid(Character))
	{
		if (!ValidateBuyRequest(ID, Character)) 
		{
			return;
		}

		if (const AMyPlayerState* MyPlayerState = GetPlayerContext().GetPlayerState<AMyPlayerState>())
		{
			if (const UC_Buy* BuyComponent = MyPlayerState->GetBuyComponent())
			{
				BuyComponent->BuyWeapon(Character, ID);
			}
		}
	}

}

void UMyBuyMenuWidget::UpdateMoney(const int32 Money)
{
	CurrentMoney->SetText(FText::FromString(FString::Printf(TEXT("Money: %d"), Money)));
}
