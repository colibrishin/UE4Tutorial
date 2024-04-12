// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyBombProgressWidget.h"

#include "MyC4.h"
#include "Components/ProgressBar.h"

void UMyBombProgressWidget::SetValue(const float Value) const
{
	ProgressBar->SetPercent(Value);
}

void UMyBombProgressWidget::BindBomb(const AMyC4* NewBomb)
{
	Bomb = NewBomb;
}

void UMyBombProgressWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Bomb.IsValid())
	{
		if (Bomb->BeingDefused())
		{
			SetRenderOpacity(1.f);
			SetValue(1.f - Bomb->GetDefusingRatio());
		}
		else if (Bomb->BeingPlanted())
		{
			SetRenderOpacity(1.f);
			UE_LOG(LogTemp, Warning, TEXT("Planting Ratio: %f"), Bomb->GetPlantingRatio());
			SetValue(Bomb->GetPlantingRatio());
		}
		else
		{
			SetRenderOpacity(0.f);
		}
	}
	else
	{
		SetRenderOpacity(0.f);
	}
}
