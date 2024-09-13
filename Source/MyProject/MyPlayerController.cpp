// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyPlayerController.h"

#include "MyAimableWeapon.h"
#include "Private/CommonBuy.hpp"
#include "MyCameraManager.h"
#include "MyCharacter.h"
#include "MyGrenade.h"
#include "MyInGameHUD.h"
#include "MyMeleeWeapon.h"
#include "MyProject/Widgets/MyInGameWidget.h"
#include "MyPlayerState.h"
#include "MySpectatorPawn.h"
#include "MyWeapon.h"
#include "MyWeaponDataAsset.h"

#include "GameFramework/GameStateBase.h"
#include "Interfaces/MyPlayerStateRequiredWidget.h"
#include "Net/UnrealNetwork.h"

AMyPlayerController::AMyPlayerController()
{
	PlayerCameraManagerClass = AMyCameraManager::StaticClass();
}

void AMyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AMyPlayerState* MyPlayerState = Cast<AMyPlayerState>(PlayerState);

	if (!IsValid(MyPlayerState))
	{
		return;
	}

	// Player state replication would be happened in client side and player controller exists in local player. 
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

void AMyPlayerController::SetSpectator(AMySpectatorPawn* Spectator)
{
	UnPossess();
	Possess(Spectator);
}


void AMyPlayerController::Client_SetSpectator_Implementation(AMySpectatorPawn* Spectator)
{
	SetSpectator(Spectator);
}