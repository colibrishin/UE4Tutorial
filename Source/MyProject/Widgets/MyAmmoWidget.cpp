// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAmmoWidget.h"

#include "../MyAimableWeapon.h"
#include "../MyPlayerState.h"
#include "../MyWeapon.h"
#include "../Components/MyWeaponStatComponent.h"

#include "Components/TextBlock.h"

void UMyAmmoWidget::DispatchPlayerState(AMyPlayerState* InPlayerState)
{
	check(InPlayerState);
	InPlayerState->OnHandChanged.AddUniqueDynamic(this, &UMyAmmoWidget::HandleWeaponChanged);
}

void UMyAmmoWidget::UpdateAmmo(const int32 CurrentAmmoCount, const int32 RemainingAmmoCount)
{
	const FText Formatted = FText::Format(FText::FromString(TEXT("{0}/{1}")), 
	                                     FText::AsNumber(CurrentAmmoCount), 
	                                     FText::AsNumber(RemainingAmmoCount));

	AmmoText->SetText(Formatted);
}

void UMyAmmoWidget::HandleWeaponChanged(AMyCollectable* InPrevious, AMyCollectable* InNew, AMyPlayerState* InPlayerState)
{
	if (const AMyAimableWeapon* PreviousWeapon = Cast<AMyAimableWeapon>(InPrevious))
	{
		AmmoText->SetText(FText::GetEmpty());
		PreviousWeapon->GetWeaponStatComponent()->OnAmmoConsumed.RemoveAll(this);
	}

	if (const AMyAimableWeapon* NewWeapon = Cast<AMyAimableWeapon>(InNew))
	{
		NewWeapon->GetWeaponStatComponent()->OnAmmoConsumed.AddDynamic(this, &UMyAmmoWidget::UpdateAmmo);
		UpdateAmmo(NewWeapon->GetWeaponStatComponent()->GetCurrentAmmoCount(),
			NewWeapon->GetWeaponStatComponent()->GetRemainingAmmoCount());
	}
}

