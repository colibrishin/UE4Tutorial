// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/MyWeapon.h"
#include "MyMeleeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyMeleeWeapon : public AMyWeapon
{
	GENERATED_BODY()

protected:
	virtual bool AttackImpl() override;
	virtual bool ReloadImpl() override;
	virtual void OnFireRateTimed() override;
	virtual void OnReloadDone() override;
	
};
