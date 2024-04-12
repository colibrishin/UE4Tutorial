// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utilities.hpp"

#include "MyProject/MyWeapon.h"
#include "MyAimableWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyAimableWeapon : public AMyWeapon
{
	GENERATED_BODY()

public:
	AMyAimableWeapon();

	virtual bool Interact(AMyCharacter* Character) override;


protected:
	virtual bool AttackImpl() override;

	virtual bool ReloadImpl() override;

	virtual void OnFireRateTimed() override;
	virtual void OnReloadDone() override;

private:
	void UpdateAmmoDisplay() const;
};
