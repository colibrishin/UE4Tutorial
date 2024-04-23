// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyCharacterWidget.h"

#include "MyPlayerState.h"
#include "MyStatComponent.h"

#include "Components/ProgressBar.h"

void UMyCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMyCharacterWidget::BindPlayerState(AMyPlayerState* MyPlayerState)
{
	MyPlayerState->BindOnHPChanged(this, &UMyCharacterWidget::UpdateHpRatio);
}

void UMyCharacterWidget::UpdateHpRatio(const float Value) const
{
	if (!IsValid(HPProgressBar)) return;

	HPProgressBar->SetPercent(Value);
}
