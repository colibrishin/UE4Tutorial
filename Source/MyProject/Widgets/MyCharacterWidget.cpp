// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterWidget.h"

#include "../MyPlayerState.h"
#include "Components/ProgressBar.h"

void UMyCharacterWidget::DispatchPlayerState(AMyPlayerState* InPlayerState)
{
	InPlayerState->OnHPChanged.AddUniqueDynamic(this, &UMyCharacterWidget::UpdateHpRatio);
}

void UMyCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMyCharacterWidget::UpdateHpRatio(const float Value)
{
	if (!IsValid(HPProgressBar)) return;

	HPProgressBar->SetPercent(Value);
}
