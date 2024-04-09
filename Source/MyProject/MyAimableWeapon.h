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

	void Fire();

	DECL_BINDON(OnFireReady)

private:
	void ResetFire();

	UPROPERTY(VisibleAnywhere)
	float Range = 1000.0f;

	UPROPERTY(VisibleAnywhere)
	int32 Ammo = 100;

	UPROPERTY(VisibleAnywhere)
	int32 MaxAmmo = 100;

	UPROPERTY(VisibleAnywhere)
	int32 Magazine = 10;

	UPROPERTY(VisibleAnywhere)
	float FireRate = 0.1f;

	UPROPERTY(VisibleAnywhere)
	bool bCanFire;

	FTimerHandle FireRateTimerHandle;

	FOnFireReady OnFireReady;
	
};
