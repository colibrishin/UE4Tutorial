// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyAmmoWidget.h"

#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "MyWeapon.h"

#include "Components/TextBlock.h"

void UMyAmmoWidget::UpdateAmmo(const int32 CurrentAmmoCount, const int32 RemainingAmmoCount) const
{
	const FText Formatted = FText::Format(FText::FromString(TEXT("{0}/{1}")), 
	                                     FText::AsNumber(CurrentAmmoCount), 
	                                     FText::AsNumber(RemainingAmmoCount));

	AmmoText->SetText(Formatted);
}

void UMyAmmoWidget::BindPlayerState(AMyPlayerState* PlayerState) const
{
	if (IsValid(PlayerState))
	{
		PlayerState->BindOnStateChanged(this, &UMyAmmoWidget::HandleStateChanged);
		PlayerState->BindOnWeaponChanged(this, &UMyAmmoWidget::HandleWeaponChanged);
	}
}

void UMyAmmoWidget::HandleStateChanged(AMyPlayerState* PlayerState, const EMyCharacterState State) const
{
	if (State == EMyCharacterState::Dead)
	{
		AmmoText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		if (PlayerState->GetWeapon() == nullptr)
		{
			AmmoText->SetText(FText::FromString(TEXT("")));
		}

		AmmoText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMyAmmoWidget::HandleWeaponChanged(AMyPlayerState* PlayerState) const
{
	if (IsValid(PlayerState->GetWeapon()))
	{
		LOG_FUNC(LogTemp, Warning, "Weapon changed, update ammo count");
		const auto& StatComponent = PlayerState->GetWeapon()->GetWeaponStatComponent();
		UpdateAmmo(StatComponent->GetCurrentAmmoCount(), StatComponent->GetRemainingAmmoCount());
	}
	else
	{
		AmmoText->SetText(FText::FromString(TEXT("")));
	}
}
