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

	virtual bool PreInteract(AMyCharacter* Character) override;
	virtual bool PostInteract(AMyCharacter* Character) override;
	virtual bool TryAttachItem(const AMyCharacter* Character) override;
	virtual bool PreUse(AMyCharacter* Character) override;
	virtual bool PostUse(AMyCharacter* Character) override;


protected:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual bool AttackImpl() override;
	virtual bool ReloadImpl() override;

	virtual void OnFireRateTimed() override;
	virtual void OnReloadDone() override;

private:
	void UpdateAmmoDisplay() const;

	UPROPERTY(EditAnywhere, Category = "Aimable Weapon")
	class UNiagaraComponent* BulletTrail;
};
