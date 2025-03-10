// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyPlayerController.h"

#include "MyCameraManager.h"
#include "MyInGameHUD.h"
#include "MyProject/Widgets/MyInGameWidget.h"
#include "MyPlayerState.h"
#include "MySpectatorPawn.h"

#include "Actors/BaseClass/A_Character.h"

#include "MyProject/Private/Utilities.hpp"
#include "Actors/BaseClass/A_Character.h"

#include "Components/Asset/C_CharacterAsset.h"

#include "Interfaces/CharacterRequiredWidget.h"
#include "Interfaces/MyPlayerStateRequiredWidget.h"

#include "Kismet/GameplayStatics.h"

AMyPlayerController::AMyPlayerController()
{
	PlayerCameraManagerClass = AMyCameraManager::StaticClass();
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AMyPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
}

void AMyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void AMyPlayerController::ClientSetHUD_Implementation( TSubclassOf<AHUD> NewHUDClass )
{
	Super::ClientSetHUD_Implementation( NewHUDClass );

	if (GetCharacter())
	{
		UpdateHUDForPawn();
	}

	if (GetPlayerState<AMyPlayerState>())
	{
		UpdateHUDForPlayerState();
	}
}

void AMyPlayerController::UpdateHUDForPlayerState() const
{
	if ( AMyPlayerState* MyPlayerState = GetPlayerState<AMyPlayerState>() )
	{
		if ( const AMyInGameHUD* HUD = Cast<AMyInGameHUD>( GetHUD() ) )
		{
			// Iterate all widgets and pass the own player state to the widgets that require the player state. 
			for ( TFieldIterator<FObjectProperty> Iterator( UMyInGameWidget::StaticClass() );
				  Iterator;
				  ++Iterator )
			{
				UUserWidget* Widget = Cast<UUserWidget>(
					Iterator->GetObjectPropertyValue(
						Iterator->ContainerPtrToValuePtr<void>( HUD->GetInGameWidget() , 0 )
					)
				);
				if ( IMyPlayerStateRequiredWidget* Interface = Cast<IMyPlayerStateRequiredWidget>( Widget ) )
				{
					Interface->DispatchPlayerState( Cast<AMyPlayerState>( MyPlayerState ) );
				}
			}
		}
	}
}

void AMyPlayerController::UpdateHUDForPawn() const
{
	if ( AA_Character* MyCharacter = Cast<AA_Character>( GetCharacter() ) )
	{
		LOG_FUNC_PRINTF( LogPlayerController , Log , "Dispatch character %s to HUD" , *GetCharacter()->GetName() );

		if ( const AMyInGameHUD* HUD = Cast<AMyInGameHUD>( GetHUD() ) )
		{
			// Iterate all widgets and pass the own player state to the widgets that require the character. 
			for ( TFieldIterator<FObjectProperty> Iterator( UMyInGameWidget::StaticClass() );
				  Iterator;
				  ++Iterator )
			{
				UUserWidget* Widget = Cast<UUserWidget>(
					Iterator->GetObjectPropertyValue( Iterator->ContainerPtrToValuePtr<void>( HUD->GetInGameWidget() , 0 ) )
				);
				if ( ICharacterRequiredWidget* Interface = Cast<ICharacterRequiredWidget>( Widget ) )
				{
					Interface->DispatchCharacter( MyCharacter );
				}
			}
		}
	}
}

void AMyPlayerController::SetSpectator(AMySpectatorPawn* Spectator)
{
	UnPossess();
	Possess(Spectator);
}

void AMyPlayerController::Client_SetSpectator_Implementation(AMySpectatorPawn* Spectator)
{
	SetSpectator(Spectator);
}
