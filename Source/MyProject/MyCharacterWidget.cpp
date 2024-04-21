// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyCharacterWidget.h"

#include "MyPlayerState.h"
#include "MyStatComponent.h"

#include "Components/ProgressBar.h"

void UMyCharacterWidget::BindHp(AMyPlayerState* State)
{
	State->BindOnHPChanged(this, &UMyCharacterWidget::UpdateHpRatio);
}

void UMyCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const auto& PlayerState = GetPlayerContext().GetPlayerState<AMyPlayerState>();

	if (IsValid(PlayerState))
	{
		BindHp(PlayerState);
	}
}

void UMyCharacterWidget::UpdateHpRatio(const int32 PlayerId, const float Value) const
{
	if (!IsValid(HPProgressBar)) return;

	HPProgressBar->SetPercent(Value);
}
