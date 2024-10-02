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

#include "Kismet/GameplayStatics.h"

AMyPlayerController::AMyPlayerController()
{
	PlayerCameraManagerClass = AMyCameraManager::StaticClass();
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AMyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (!IsLocalPlayerController())
	{
		return;
	}

	AsyncTask
		(
		 ENamedThreads::AnyNormalThreadHiPriTask , [this]()
		 {
			 const AMyInGameHUD* HUD = Cast<AMyInGameHUD>(GetHUD());

			 while (HUD == nullptr || !HUD->GetInGameWidget()->IsValidLowLevelFast())
			 {
				 HUD = Cast<AMyInGameHUD>(GetHUD());
				 FPlatformProcess::YieldThread();
			 }

			 // Iterate all widgets and pass the own player state to the widgets that require the player state. 
			 for (TFieldIterator<FObjectProperty> Iterator(UMyInGameWidget::StaticClass());
			      Iterator;
			      ++Iterator)
			 {
				 UUserWidget* Widget = Cast<UUserWidget>
					 (
					  Iterator->GetObjectPropertyValue
					  (
					   Iterator->ContainerPtrToValuePtr<void>(HUD->GetInGameWidget() , 0)
					  )
					 );
				 if (IMyPlayerStateRequiredWidget* Interface = Cast<IMyPlayerStateRequiredWidget>(Widget))
				 {
					 Interface->DispatchPlayerState(Cast<AMyPlayerState>(PlayerState));
				 }
			 }
		 }
		);
}

void AMyPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if (!IsLocalPlayerController())
	{
		return;
	}
	
	AsyncTask
			(
			 ENamedThreads::AnyNormalThreadHiPriTask , [this]()
			 {
				 if (AA_Character* NewCharacter = Cast<AA_Character>(GetCharacter()))
				 {
					 if (const AMyInGameHUD* HUD = Cast<AMyInGameHUD>(GetHUD()))
					 {
						 while (!HUD || !HUD->GetInGameWidget()->IsValidLowLevelFast())
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

void AMyPlayerController::SetSpectator(AMySpectatorPawn* Spectator)
{
	UnPossess();
	Possess(Spectator);
}

void AMyPlayerController::Client_SetSpectator_Implementation(AMySpectatorPawn* Spectator)
{
	SetSpectator(Spectator);
}
