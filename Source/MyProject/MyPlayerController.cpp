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

void AMyPlayerController::OnPossess( APawn* aPawn )
{
	Super::OnPossess( aPawn );
	
	// Case for the listen server.
	if ( HasAuthority() && aPawn ) 
	{
		UpdateHUDForPawn();
	}
}

void AMyPlayerController::InitPlayerState()
{
	Super::InitPlayerState();

	// Case for the listen server.
	if ( HasAuthority() )
	{
		UpdateHUDForPlayerState();
	}
}

void AMyPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	// HUD is initialized before the pawn. Also handles the GameModeBase::RestartPlayer cases.
	if ( GetCharacter() )
	{
		UpdateHUDForPawn();
	}
}

void AMyPlayerController::ClientSetHUD_Implementation( TSubclassOf<AHUD> NewHUDClass )
{
	Super::ClientSetHUD_Implementation( NewHUDClass );

	// Player State will be initialized along with the Player Controller.
	if ( GetPlayerState<AMyPlayerState>() )
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
				void* PropertyAddress = Iterator->ContainerPtrToValuePtr<void>( HUD->GetInGameWidget() );
				UObject* ObjectPointer = Iterator->GetObjectPropertyValue( PropertyAddress );
				UUserWidget* Widget = Cast<UUserWidget>( ObjectPointer );

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
				void* PropertyAddress = Iterator->ContainerPtrToValuePtr<void>(HUD->GetInGameWidget());
				UObject* ObjectPointer = Iterator->GetObjectPropertyValue( PropertyAddress );
				UUserWidget* Widget = Cast<UUserWidget>( ObjectPointer );

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
