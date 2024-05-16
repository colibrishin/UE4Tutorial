// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyWeapon.h"

#include "MyProject/MyItem.h"
#include "MyFragGrenade.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyFragGrenade : public AMyWeapon
{
	GENERATED_BODY()

public:
    AMyFragGrenade();

	class AMyCharacter* GetPreviousOwner() const { return PreviousOwner.Get(); }

protected:
	virtual bool AttackImpl() override;

	virtual bool ReloadImpl() override;

	virtual void DropLocation() override;

private:
	void Throw();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_Throw();

	void ThrowImpl();

	void OnExplosionTimerExpired();

	UPROPERTY(VisibleAnywhere)
	bool IsThrown;

	UPROPERTY(VisibleAnywhere)
	bool IsExploded;

	FTimerHandle OnExplosionTimerExpiredHandle;

	TWeakObjectPtr<class AMyCharacter> PreviousOwner;

};
