// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/MyAimableWeapon.h"
#include "MyCabinWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyCabinWeapon : public AMyAimableWeapon
{
	GENERATED_BODY()

	AMyCabinWeapon();

protected:
	virtual bool AttackImpl() override;
	virtual bool ReloadImpl() override;

private:

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class USoundBase* ReloadSound;
	
};
