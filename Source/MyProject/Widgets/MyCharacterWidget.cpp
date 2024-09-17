// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterWidget.h"

#include "../MyPlayerState.h"
#include "Components/ProgressBar.h"

#include "MyProject/Components/C_Health.h"

void UMyCharacterWidget::DispatchPlayerState(AMyPlayerState* InPlayerState)
{
	InPlayerState->GetHealthComponent()->OnHPChangedRatio.AddUniqueDynamic(this, &UMyCharacterWidget::UpdateHpRatio);
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
