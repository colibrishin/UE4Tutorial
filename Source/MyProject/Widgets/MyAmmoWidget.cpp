// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAmmoWidget.h"

#include "../MyPlayerState.h"
#include "Components/TextBlock.h"
#include "MyProject/Private/Utilities.hpp"

#include "MyProject/Components/Weapon/C_Weapon.h"

DEFINE_LOG_CATEGORY(LogAmmoWidget);

void UMyAmmoWidget::DispatchCharacter(AA_Character* InCharacter)
{
	check(InCharacter);
	InCharacter->OnHandChanged.AddUObject(this, &UMyAmmoWidget::HandleWeaponChanged);
}

void UMyAmmoWidget::UpdateAmmo(const int32 CurrentAmmoCount, const int32 RemainingAmmoCount, UC_Weapon* /*InWeapon*/)
{
	const FText Formatted = FText::Format(FText::FromString(TEXT("{0}/{1}")), 
	                                     FText::AsNumber(CurrentAmmoCount), 
	                                     FText::AsNumber(RemainingAmmoCount));

	AmmoText->SetText(Formatted);
}

void UMyAmmoWidget::HandleWeaponChanged(UC_PickUp* InPrevious, UC_PickUp* InNew)
{
	LOG_FUNC(LogAmmoWidget, Log, "Caught weapon change");

	if (InPrevious)
	{
		if (UC_Weapon* PreviousWeapon = InPrevious->GetOwner()->GetComponentByClass<UC_Weapon>())
		{
			AmmoText->SetText(FText::GetEmpty());
			PreviousWeapon->OnAmmoUpdated.RemoveAll(this);
		}
	}

	if (InNew)
	{
		if (UC_Weapon* NewWeapon = InNew->GetOwner()->GetComponentByClass<UC_Weapon>())
		{
			NewWeapon->OnAmmoUpdated.AddUniqueDynamic(this, &UMyAmmoWidget::UpdateAmmo);
			UpdateAmmo(
				NewWeapon->GetRemainingAmmoInClip(),
			           NewWeapon->GetRemainingAmmoWithoutCurrentClip(),
			           NewWeapon);
		}
	}
}

