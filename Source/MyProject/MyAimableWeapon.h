// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	virtual void Client_TryAttachItem_Implementation(AMyCharacter* Character) override;

	virtual void DropBeforeCharacter() override;

	virtual void BeginPlay() override;

	virtual bool AttackImpl() override;
	virtual bool ReloadImpl() override;

	virtual void OnFireRateTimed() override;
	virtual void OnReloadDone() override;

	static float GetCurveValue(const UCurveFloat* InCurve, const float InValue, const float InValueMax, const float Min = -0.9999f, const float Max = 0.9999f);

private:
	UFUNCTION(Reliable, NetMulticast)
	void Multi_TriggerBulletTrail();
	
	UFUNCTION(Reliable, Client)
	void Client_Attack();

	UFUNCTION(Reliable, Client)
	void Client_Reload();

	UFUNCTION(Reliable, Client)
	void Client_DropBeforeCharacter();

	UPROPERTY(VisibleAnywhere, Replicated)
	FVector Normal;

	UPROPERTY(EditAnywhere, Category = "Aimable Weapon")
	class UNiagaraComponent* BulletTrail;
};
