// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/MyWeapon.h"
#include "MyGrenade.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyGrenade : public AMyWeapon
{
	GENERATED_BODY()

public:
	
	class AMyCharacter* GetPreviousOwner() const { return PreviousOwner.Get(); }

protected:
	virtual bool AttackImpl() override;
	virtual bool AttackInterruptedImpl() override;

	virtual bool ReloadImpl() override;

	virtual void DropLocation() override;

	virtual void OnExplosionTimerExpiredImpl();

private:
	UFUNCTION(Client, Reliable)
	void Client_CookThrowable();

	void Throw();

	void Charge();

	void OnExplosionTimerExpired();

	UPROPERTY(VisibleAnywhere)
	bool IsThrown;

	UPROPERTY(VisibleAnywhere)
	bool IsExploded;

	FTimerHandle OnExplosionTimerExpiredHandle;

	TWeakObjectPtr<class AMyCharacter> PreviousOwner;
	
};
