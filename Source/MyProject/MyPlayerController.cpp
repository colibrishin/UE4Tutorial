// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyPlayerController.h"

#include "MyCameraManager.h"
#include "MyInGameHUD.h"
#include "MyProject/Widgets/MyInGameWidget.h"
#include "MyPlayerState.h"
#include "MySpectatorPawn.h"

#include "Interfaces/MyPlayerStateRequiredWidget.h"

AMyPlayerController::AMyPlayerController()
{
	PlayerCameraManagerClass = AMyCameraManager::StaticClass();
}

void AMyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (GetWorld()->GetFirstPlayerController() != this)
	{
		return;
	}

	AMyPlayerState* MyPlayerState = Cast<AMyPlayerState>(PlayerState);

	if (!IsValid(MyPlayerState))
	{
		return;
	}

	// fixme: workaround for avoiding ContainerPtr assertion;
	// spin lock;
	while (!MyPlayerState->IsValidLowLevel()) {}

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

void AMyPlayerController::OnRep_Pawn()
{
	if (GetWorld()->GetFirstPlayerController() != this)
	{
		Super::OnRep_Pawn();
		return;
	}

	const AMyPlayerState* MyPlayerState = Cast<AMyPlayerState>(PlayerState);
	AA_Character* OldCharacter = Cast<AA_Character>(GetCharacter());
	ensure(MyPlayerState);
	ensure(OldCharacter);

	if (OldCharacter && CharacterForwardHandle.IsValid())
	{
		OldCharacter->OnHandChanged.Remove(CharacterForwardHandle);
	}
	CharacterForwardHandle = {};
	
	Super::OnRep_Pawn();

	if (AA_Character* NewCharacter = Cast<AA_Character>(GetCharacter()))
	{
		CharacterForwardHandle = NewCharacter->OnHandChanged.AddRaw(
			&MyPlayerState->OnHandChanged,
			&FOnHandChangedDynamic::Broadcast);
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