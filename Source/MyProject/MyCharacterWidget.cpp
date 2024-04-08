// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyCharacterWidget.h"

#include "MyStatComponent.h"

#include "Components/ProgressBar.h"

void UMyCharacterWidget::BindHp(UMyStatComponent* Component)
{
	Component->BindOnHPChanged(this, &UMyCharacterWidget::UpdateHpRatio);
}

void UMyCharacterWidget::UpdateHpRatio(float Value) const
{
	if (!IsValid(HPProgressBar)) return;

	HPProgressBar->SetPercent(Value);
}
