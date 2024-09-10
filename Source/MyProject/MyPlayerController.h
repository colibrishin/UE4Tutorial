// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

	void BuyWeapon(const int32 WeaponID) const;
	void SetSpectator(class AMySpectatorPawn* Spectator);

	UFUNCTION(Client, Reliable)
	void Client_SetSpectator(class AMySpectatorPawn* Spectator);

protected:
	virtual void OnRep_PlayerState() override;

private:
	UFUNCTION(Server, Reliable)
	void Server_BuyWeapon(class AMyCharacter* RequestCharacter, const int32 WeaponID) const;

	void ProcessBuy(class AMyCharacter* RequestCharacter, const int32 WeaponID) const;

};
