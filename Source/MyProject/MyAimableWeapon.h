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
	virtual bool TryAttachItem(AMyCharacter* Character) override;
	virtual bool PreUse(AMyCharacter* Character) override;
	virtual bool PostUse(AMyCharacter* Character) override;

	bool Hitscan(IN const FVector& Position, IN const FVector& Forward, OUT FHitResult& OutHitResult);

protected:
	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual bool AttackImpl() override;
	virtual bool ReloadImpl() override;

	virtual void OnFireRateTimed() override;
	virtual void OnReloadDone() override;

	float GetHSpreadDegree(const float Point, const float OscillationRate, const float Min = 0, const float Max = PI / 8.f);
	float GetVSpreadDegree(const float Point, const float OscillationRate, const float Min = 0, const float Max = PI / 8.f);

private:
	UFUNCTION(Reliable, NetMulticast)
	void Multi_TriggerBulletTrail();

	UFUNCTION(Reliable, Client)
	void Client_UpdateAmmoDisplay() const;

	UFUNCTION(Reliable, Client)
	void Client_Attack();

	UFUNCTION(Reliable, Client)
	void Client_Reload();

	UPROPERTY(VisibleAnywhere, Replicated)
	FVector Normal;

	UPROPERTY(EditAnywhere, Category = "Aimable Weapon")
	class UNiagaraComponent* BulletTrail;
};
