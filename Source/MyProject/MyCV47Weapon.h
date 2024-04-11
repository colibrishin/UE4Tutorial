// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/MyAimableWeapon.h"
#include "MyCV47Weapon.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyCV47Weapon : public AMyAimableWeapon
{
	GENERATED_BODY()

	AMyCV47Weapon();

protected:
	virtual bool AttackImpl() override;
	virtual bool ReloadImpl() override;

private:

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class USoundBase* ReloadSound;

};
