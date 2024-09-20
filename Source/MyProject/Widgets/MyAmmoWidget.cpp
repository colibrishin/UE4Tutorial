// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAmmoWidget.h"

#include "../MyPlayerState.h"
#include "Components/TextBlock.h"

#include "MyProject/Components/Weapon/C_Weapon.h"

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
	if (UC_Weapon* PreviousWeapon = Cast<UC_Weapon>(InPrevious))
	{
		AmmoText->SetText(FText::GetEmpty());
		PreviousWeapon->OnAmmoUpdated.RemoveAll(this);
	}

	if (UC_Weapon* NewWeapon = Cast<UC_Weapon>(InNew->GetOwner()->GetComponentByClass<UC_Weapon>()))
	{
		NewWeapon->OnAmmoUpdated.AddUniqueDynamic(this, &UMyAmmoWidget::UpdateAmmo);
		UpdateAmmo(
			NewWeapon->GetRemainingAmmoInClip(),
		           NewWeapon->GetRemainingAmmoWithoutCurrentClip(),
		           NewWeapon);
	}
}

