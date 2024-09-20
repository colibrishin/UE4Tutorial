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

void AMyPlayerController::SetSpectator(AMySpectatorPawn* Spectator)
{
	UnPossess();
	Possess(Spectator);
}


void AMyPlayerController::Client_SetSpectator_Implementation(AMySpectatorPawn* Spectator)
{
	SetSpectator(Spectator);
}