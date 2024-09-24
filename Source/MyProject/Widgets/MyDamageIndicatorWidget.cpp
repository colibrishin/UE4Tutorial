// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDamageIndicatorWidget.h"
#include "MyProject/MyPlayerState.h"
#include "Components/Image.h"

void UMyDamageIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetRenderOpacity(0.f);
}

void UMyDamageIndicatorWidget::DispatchPlayerState(AMyPlayerState* InPlayerState)
{
	ensure(InPlayerState);
	InPlayerState->OnTakeAnyDamage.AddUniqueDynamic(this, &UMyDamageIndicatorWidget::HandleDamageTaken);
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

void UMyDamageIndicatorWidget::HandleDamageTaken(
	AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	const auto& Victim = GetPlayerContext().GetPlayerState<AMyPlayerState>();
	const auto& Normal = DamageCauser->GetActorLocation() - Victim->GetPawn()->GetActorLocation();

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
