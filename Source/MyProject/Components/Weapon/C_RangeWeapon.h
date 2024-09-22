// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_Weapon.h"
#include "C_RangeWeapon.generated.h"


UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_RangeWeapon : public UC_Weapon
{
	GENERATED_BODY()

public:
	friend class UC_WeaponAsset;
	
	// Sets default values for this component's properties
	UC_RangeWeapon();

	FVector GetRecoiledNormal() const { return RecoiledNormal; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void AttackImplementation() override;

	virtual void StopAttackImplementation() override;

	FVector ApplyRecoil(const FVector& InNormal) const;
	
	void DoHitscan(const FVector& InRecoiledNormal);
	
private:
	UPROPERTY(VisibleAnywhere, Replicated, meta=(AllowPrivateAccess))
	FVector_NetQuantize RecoiledNormal;
	
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	bool bAimable;

	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	bool bHitscan;
	
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	UCurveFloat* VSpread;

	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	UCurveFloat* HSpread;
};
