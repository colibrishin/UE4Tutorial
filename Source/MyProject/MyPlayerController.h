// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

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

	void SetSpectator(class AMySpectatorPawn* Spectator);

	UFUNCTION(Client, Reliable)
	void Client_SetSpectator(AMySpectatorPawn* Spectator);
	
};
