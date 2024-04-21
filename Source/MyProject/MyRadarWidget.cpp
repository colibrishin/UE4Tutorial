// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyRadarWidget.h"

#include "MyGameState.h"
#include "MyPlayerState.h"
#include "MySpectatorPawn.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

UMyRadarWidget::UMyRadarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetRenderOpacity(0.5f);

	static ConstructorHelpers::FObjectFinder<UTexture2D> AllyDotTexture(TEXT("Texture2D'/Game/Models/Basic_green_dot.Basic_green_dot'"));

	if (AllyDotTexture.Succeeded())
	{
		TeammateTexture = AllyDotTexture.Object;

		TeammateBrush.SetResourceObject(TeammateTexture);
		TeammateBrush.SetImageSize({16.f, 16.f});
		TeammateBrush.DrawAs = ESlateBrushDrawType::Image;
		TeammateBrush.Tiling = ESlateBrushTileType::NoTile;
		TeammateBrush.Mirroring = ESlateBrushMirrorType::NoMirror;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> EnemyDotTexture(TEXT("Texture2D'/Game/Models/Basic_red_dot.Basic_red_dot'"));

	if (EnemyDotTexture.Succeeded())
	{
		EnemyTexture = EnemyDotTexture.Object;
		EnemyBrush.SetResourceObject(EnemyTexture);
		EnemyBrush.SetImageSize({16.f, 16.f});
		EnemyBrush.DrawAs = ESlateBrushDrawType::Image;
		EnemyBrush.Tiling = ESlateBrushTileType::NoTile;
		EnemyBrush.Mirroring = ESlateBrushMirrorType::NoMirror;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> NullTextureFinder(TEXT("Texture2D'/Game/Models/null-brush.null-brush'"));

	if (NullTextureFinder.Succeeded())
	{
		NullTexture = NullTextureFinder.Object;
		NullBrush.SetResourceObject(NullTexture);
		NullBrush.SetImageSize({16.f, 16.f});
		NullBrush.DrawAs = ESlateBrushDrawType::Image;
		NullBrush.Tiling = ESlateBrushTileType::NoTile;
		NullBrush.Mirroring = ESlateBrushMirrorType::NoMirror;
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

			const FVector LocalPlayer3DLocation = PlayerController->GetPawn()->GetActorLocation();
			const auto& PlayerYaw = PlayerController->GetControlRotation().Yaw;
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
				const auto& Player = Cast<AMyPlayerState>(GameState->PlayerArray[i]);

				if (Player == GetPlayerContext().GetPlayerState())
				{
					const auto& Image = RadarImages[i];

					Image->SetBrush(TeammateBrush);
					Image->SetRenderTranslation(RadarCenter);
					continue;
				}

				if (Cast<AMySpectatorPawn>(Player->GetPawn()))
				{
					continue;
				}

				// Endpoint - StartPoint
				const auto& RelativePosition = LocalPlayer3DLocation - Player->GetPawn()->GetActorLocation();
				const auto& Relative2DPosition = FVector2D(RelativePosition);

				// Gets the angle between the two points in degrees 
				const auto& DirAngle = FMath::RadiansToDegrees(FMath::Atan2(Relative2DPosition.Y, Relative2DPosition.X));

				// Radian moves counter-clockwise and starts from "East" (0 degree).
				constexpr float EastToNorth = 90.f;

				// DirAngle - PlayerYaw = 0 if the player is facing the enemy.
				const auto& Angle = DirAngle - PlayerYaw + EastToNorth;

				// Creates a direction vector from the angle.
				const auto& Dir = FVector2D(FMath::Cos(FMath::DegreesToRadians(Angle)), FMath::Sin(FMath::DegreesToRadians(Angle)));

				// Recreates a rotated vector by multiplying the direction vector by the distance between the two points.
				const auto& RotatedVector = Dir * Relative2DPosition.Size();

				const auto& RadarPosition = RotatedVector / 10.f;
				const auto& PositionFromCenter = RadarPosition + RadarCenter;

				const auto& Distance = FVector2D(RadarPosition.X, RadarPosition.Y).Size();

				const auto& Brush = LocalPlayerTeam == Player->GetTeam() ? TeammateBrush : EnemyBrush;
				const auto& Image = RadarImages[i];

				if (Distance > RadarScale / 2.f)
				{
					Image->SetBrush(NullBrush);
					Image->SetRenderTranslation(RadarCenter);
					continue;
				}

				Image->SetBrush(Brush);
				Image->SetRenderTranslation(PositionFromCenter);
			}
		}
	}
}
