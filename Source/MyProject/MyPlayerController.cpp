// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyPlayerController.h"

#include "MyCameraManager.h"
#include "MyInGameHUD.h"
#include "MyProject/Widgets/MyInGameWidget.h"
#include "MyPlayerState.h"
#include "MySpectatorPawn.h"
#include "MyProject/Private/Utilities.hpp"

#include "Components/Asset/C_CharacterAsset.h"

#include "Interfaces/CharacterRequiredWidget.h"
#include "Interfaces/MyPlayerStateRequiredWidget.h"

AMyPlayerController::AMyPlayerController()
{
	PlayerCameraManagerClass = AMyCameraManager::StaticClass();
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_Client && GetWorld()->GetFirstPlayerController() == this)
	{
		OnPossessedPawnChanged.AddUniqueDynamic(this, &AMyPlayerController::DispatchPlayerCharacter);
	}
}

void AMyPlayerController::SetSpectator(AMySpectatorPawn* Spectator)
{
	UnPossess();
	Possess(Spectator);
}

void AMyPlayerController::DispatchPlayerCharacter(APawn* /*InOldPawn*/, APawn* /*InNewPawn*/)
{
	AsyncTask
			(
			 ENamedThreads::AnyHiPriThreadHiPriTask , [this]()
			 {
				 if (AA_Character* NewCharacter = Cast<AA_Character>(GetCharacter()))
				 {
					 if (const AMyInGameHUD* HUD = Cast<AMyInGameHUD>(GetHUD()))
					 {
						 while (!HUD)
						 {
							 HUD = Cast<AMyInGameHUD>(GetHUD());
							 FPlatformProcess::YieldThread();
						 }

						 LOG_FUNC_PRINTF
						 (LogPlayerController , Log , "Dispatch character %s to HUD" , *NewCharacter->GetName());

						 // Iterate all widgets and pass the own player state to the widgets that require the character. 
						 for (TFieldIterator<FObjectProperty> Iterator(UMyInGameWidget::StaticClass());
						      Iterator;
						      ++Iterator)
						 {
							 UUserWidget* Widget = Cast<UUserWidget>
									 (
									  Iterator->GetObjectPropertyValue
									  (Iterator->ContainerPtrToValuePtr<void>(HUD->GetInGameWidget() , 0))
									 );
							 if (ICharacterRequiredWidget* Interface = Cast<ICharacterRequiredWidget>(Widget))
							 {
								 Interface->DispatchCharacter(NewCharacter);
							 }
						 }
					 }
				 }
			 }
			);
}

void AMyPlayerController::Client_SetSpectator_Implementation(AMySpectatorPawn* Spectator)
{
	SetSpectator(Spectator);
}
