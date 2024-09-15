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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void Server_Attack_Implementation() override;

	virtual void Server_StopAttack_Implementation() override;

	FVector ApplyRecoil(const FVector& InNormal) const;
	
	void DoHitscan(const FVector& InRecoiledNormal);
	
private:
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	FVector Normal;
	
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	bool bAimable;

	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	bool bHitscan;
	
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	UCurveFloat* VSpread;

	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	UCurveFloat* HSpread;
};
