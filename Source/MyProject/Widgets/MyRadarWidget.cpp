// Fill out your copyright notice in the Description page of Project Settings.


#include "MyRadarWidget.h"

#include "MyProject/MyGameState.h"
#include "MyProject/MyPlayerState.h"
#include "MyProject/MySpectatorPawn.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

UMyRadarWidget::UMyRadarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetRenderOpacity(0.5f);

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> AllyDotTexture(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Models/M_RadarDot_Ally.M_RadarDot_Ally'"));

	if (AllyDotTexture.Succeeded())
	{
		TeammateMaterial = AllyDotTexture.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> EnemyDotTexture(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Models/M_RadarDot_Enemy.M_RadarDot_Enemy'"));

	if (EnemyDotTexture.Succeeded())
	{
		EnemyMaterial = EnemyDotTexture.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> NullTextureFinder(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Models/M_RadarDot_Null.M_RadarDot_Null'"));

	if (NullTextureFinder.Succeeded())
	{
		NullMaterial = NullTextureFinder.Object;
	}
}

void UMyRadarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry , InDeltaTime);

	// todo: Optimize
	if (const auto& GameState = GetWorld()->GetGameState<AMyGameState>())
	{
		if (RadarImages.Num() != GameState->PlayerArray.Num())
		{
			RadarImages.SetNum(GameState->PlayerArray.Num());

			for (int i = 0; i < RadarImages.Num(); ++i)
			{
				RadarImages[i] = NewObject<UImage>(this);
				const auto& ImageSlot = RadarCanvas->AddChildToCanvas(RadarImages[i]);

				ImageSlot->SetSize({16.f, 16.f});
			}
		}

		if (const auto& PlayerController = GetPlayerContext().GetPlayerController())
		{
			constexpr float RadarScale = 300.0f;

			if (PlayerController->GetPawn() == nullptr)
			{
				return;
			}

			const FVector LocalPlayer3DLocation = PlayerController->GetPawn()->GetActorLocation();

			// Local axes of the player on the XY plane (Z dropped). Forward is normalized so that
			// dot products against it return a distance, not a cosine.
			// UE top-down: X = forward, Y = right, so rotating (x, y) by +90 degrees gives (-y, x).
			const FVector2D PlayerForward = FVector2D(PlayerController->GetControlRotation().Vector()).GetSafeNormal();
			const FVector2D PlayerRight   = { -PlayerForward.Y, PlayerForward.X };
			const auto& PlayerState = Cast<AMyPlayerState>(GetPlayerContext().GetPlayerState());

			if (!IsValid(PlayerState))
			{
				return;
			}

			const EMyTeam LocalPlayerTeam = PlayerState->GetTeam();

			const FVector2D RadarSize = FVector2D(RadarScale, RadarScale);
			const FVector2D RadarCenter = FVector2D(RadarScale / 2.f, RadarScale / 2.f);

			for (int i = 0; i < GameState->PlayerArray.Num(); ++i)
			{
				const auto& OtherPlayerState = Cast<AMyPlayerState>(GameState->PlayerArray[i]);

				if (OtherPlayerState == GetPlayerContext().GetPlayerState())
				{
					const auto& Image = RadarImages[i];

					Image->SetBrushFromMaterial(TeammateMaterial);
					Image->SetRenderTranslation(RadarCenter);
					continue;
				}

				if (Cast<AMySpectatorPawn>(OtherPlayerState->GetPawn()))
				{
					continue;
				}

				if (!IsValid(OtherPlayerState))
				{
					return;
				}

				if (!IsValid(OtherPlayerState->GetPawn()))
				{
					return;
				}

				// From the local player to the other player, Z dropped (not normalized: keeps the distance).
				const FVector2D ToOther = FVector2D(OtherPlayerState->GetPawn()->GetActorLocation() - LocalPlayer3DLocation);

				// Projecting onto the player's axes gives the position in the player's frame:
				// |ToOther| * cos(theta) along each unit axis, i.e. "how far right" and "how far ahead".
				const float Right   = ToOther.Dot( PlayerRight );
				const float Forward = ToOther.Dot( PlayerForward );

				// Widget space: +X = right, +Y = down, so "ahead" maps to -Y.
				const FVector2D RadarPosition = FVector2D( Right, -Forward ) / 10.f;
				const auto& PositionFromCenter = RadarPosition + RadarCenter;

				const auto& Distance = FVector2D(RadarPosition.X, RadarPosition.Y).Size();

				const auto& Brush = LocalPlayerTeam == OtherPlayerState->GetTeam() ? TeammateMaterial : EnemyMaterial;
				const auto& Image = RadarImages[i];

				if (Distance > RadarScale / 2.f)
				{
					Image->SetBrushFromMaterial(NullMaterial);
					Image->SetRenderTranslation(RadarCenter);
					continue;
				}

				Image->SetBrushFromMaterial(Brush);
				Image->SetRenderTranslation(PositionFromCenter);
			}
		}
	}
}
