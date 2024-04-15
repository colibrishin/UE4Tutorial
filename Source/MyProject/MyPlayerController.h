// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <mutex>

#include "CoreMinimal.h"
#include "MyProjectGameModeBase.h"
#include "Utilities.hpp"

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

protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

private:
	UFUNCTION(Server, Reliable)
	void Server_BuyWeapon(class AMyCharacter* RequestCharacter, const int32 WeaponID) const;

	void ProcessBuy(class AMyCharacter* RequestCharacter, const int32 WeaponID) const;

};
