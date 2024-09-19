// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyPlayerController.h"

#include "MyCameraManager.h"
#include "MyInGameHUD.h"
#include "MyProject/Widgets/MyInGameWidget.h"
#include "MyPlayerState.h"
#include "MySpectatorPawn.h"

#include "Components/Asset/C_CharacterAsset.h"

#include "Interfaces/CharacterRequiredWidget.h"
#include "Interfaces/MyPlayerStateRequiredWidget.h"

AMyPlayerController::AMyPlayerController()
{
	PlayerCameraManagerClass = AMyCameraManager::StaticClass();
}

void AMyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	{
		AMyPlayerState* MyPlayerState = Cast<AMyPlayerState>(PlayerState);
		check(MyPlayerState);
		
		if (const AMyInGameHUD* HUD = Cast<AMyInGameHUD>(GetHUD()))
		{
			// Iterate all widgets and pass the own player state to the widgets that require the player state. 
			for (TFieldIterator<FObjectProperty> Iterator(UMyInGameWidget::StaticClass());
				Iterator;
				++Iterator)
			{
				UUserWidget* Widget = Cast<UUserWidget>(Iterator->GetObjectPropertyValue(Iterator->ContainerPtrToValuePtr<void>(HUD->GetInGameWidget(), 0)));
				if (IMyPlayerStateRequiredWidget* Interface = Cast<IMyPlayerStateRequiredWidget>(Widget))
				{
					Interface->DispatchPlayerState(MyPlayerState);
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