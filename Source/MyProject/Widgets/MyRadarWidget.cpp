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
			// Project the Control Rotation normal vector to the xy plane (Yaw Only)
			const auto& PlayerForward = FVector2D(FVector::VectorPlaneProject( PlayerController->GetControlRotation().Vector(), {1.f, 1.f, 0.f} )).GetSafeNormal();
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

				// Endpoint - StartPoint
				const auto& RelativePosition = LocalPlayer3DLocation - OtherPlayerState->GetPawn()->GetActorLocation();
				// Projection
				const auto& Relative2DPosition = FVector2D( FVector::VectorPlaneProject( RelativePosition , { 1.f, 1.f, 0.f } ) );

				// Gets the x-axis point ( |1| * |1| * cos ) and acos it to get the radian value.
				// acos = [0, pi], the sign will be lost.
				const auto& RelativeNormal = Relative2DPosition.GetSafeNormal();

				// Use the atan2 to cover the [-pi, pi], Need the dot and the cross product
				const auto& Dot = RelativeNormal.Dot( PlayerForward );
				const float Cross = FVector2D::CrossProduct( RelativeNormal, PlayerForward ); // ax*by - bx*ay

				// https://en.wikipedia.org/wiki/Atan2#East-counterclockwise,_north-clockwise_and_south-clockwise_conventions,_etc.
				// (Cross, Dot) = East-CCW
				// (Dot, Cross) = North-CW
				// (Dot, -Cross) = North-CCW
				const auto& ATanD = FMath::Atan2( Dot, -Cross ); 

				FVector2D Dir = { FMath::Cos( ATanD ), FMath::Sin( ATanD ) }; // Radar forward

				// Recreates a rotated vector by multiplying the direction vector by the distance between the two points.
				const auto& RotatedVector = Dir * Relative2DPosition.Size();

				const auto& RadarPosition = RotatedVector / 10.f;
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
