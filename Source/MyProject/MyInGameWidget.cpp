// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyInGameWidget.h"

#include "MyCharacter.h"
#include "MyCharacterWidget.h"

void UMyInGameWidget::BindPlayer(const AMyCharacter* Player) const
{
	if (IsValid(Player))
	{
		if (CharacterWidget)
		{
			CharacterWidget->BindHp(Player->GetStatComponent());
		}
	}
}
