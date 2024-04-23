// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyDamageIndicatorWidget.h"

#include "MyPlayerState.h"

#include "Components/Image.h"

void UMyDamageIndicatorWidget::BindPlayerState(AMyPlayerState* MyPlayerState)
{
	// todo: replace with native any damage function
	MyPlayerState->BindOnDamageTaken(this, &UMyDamageIndicatorWidget::HandleDamageTaken);
}

void UMyDamageIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetRenderOpacity(0.f);
}

void UMyDamageIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry , InDeltaTime);

	if (bVisible)
	{
		DeltaTime += InDeltaTime;
		SetRenderOpacity(((2.f - DeltaTime) / 2.f) * 0.5f);

		if (DeltaTime > 2.f)
		{
			bVisible = false;
			DeltaTime = 0.f;
			SetRenderOpacity(0.f);
		}
	}
}

void UMyDamageIndicatorWidget::HandleDamageTaken(AMyPlayerState* DamageGiver)
{
	if (IsValid(DamageGiver))
	{
		const auto& Victim = GetPlayerContext().GetPlayerState<AMyPlayerState>();
		const auto& Normal = DamageGiver->GetPawn()->GetActorLocation() - Victim->GetPawn()->GetActorLocation();

		const auto& Angle = FMath::RadiansToDegrees(FMath::Atan2(Normal.Y, Normal.X));

		FWidgetTransform Transform;
		Transform.Angle = Angle;
		Transform.Translation = FVector2D(0.f, 0.f);
		Transform.Scale = FVector2D(1.f, 1.f);

		IndicatorImage->SetRenderTransform(Transform);

		DeltaTime = 0.f;
		bVisible = true;
		SetRenderOpacity(0.5f);
	}
}
