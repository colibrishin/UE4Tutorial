// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utilities.hpp"

#include "MyProject/MyWeapon.h"
#include "MyAimableWeapon.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnFireReady)

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyAimableWeapon : public AMyWeapon
{
	GENERATED_BODY()

public:
	AMyAimableWeapon();

	DECL_BINDON(OnFireReady)

	virtual void Attack() override;
	virtual bool Interact(AMyCharacter* Character) override;

private:
	void ResetFire();

	UPROPERTY(VisibleAnywhere)

	FTimerHandle FireRateTimerHandle;

	FOnFireReady OnFireReady;
	
};
