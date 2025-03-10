// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class AMyPlayerState;
class AA_Character;
class UC_Buy;
/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

	virtual void BeginPlay() override;

	virtual void OnRep_Pawn() override;

	virtual void OnRep_PlayerState() override;

	virtual void ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass) override;

	void UpdateHUDForPlayerState() const;

	void UpdateHUDForPawn() const;

	void SetSpectator(class AMySpectatorPawn* Spectator);
	
	UFUNCTION(Client, Reliable)
	void Client_SetSpectator(AMySpectatorPawn* Spectator);

};
