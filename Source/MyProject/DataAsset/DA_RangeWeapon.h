// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DA_Weapon.h"
#include "DA_RangeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UDA_RangeWeapon : public UDA_Weapon
{
	GENERATED_BODY()

public:
	bool GetAimable() const { return bAimable; }
	bool GetHitscan() const { return bHitscan; }
	UCurveFloat* GetVSpread() const { return VSpread; }
	UCurveFloat* GetHSpread() const { return HSpread; }
	
private:
	UPROPERTY(EditAnywhere, Category="Range", BlueprintReadWrite, meta=(AllowPrivateAccess))
	bool bAimable;

	// todo: projectile class for non-hitscan weapon.
	UPROPERTY(EditAnywhere, Category="Range", BlueprintReadWrite, meta=(AllowPrivateAccess))
	bool bHitscan;
	
	UPROPERTY(EditAnywhere, Category="Range", BlueprintReadWrite, meta=(AllowPrivateAccess))
	UCurveFloat* VSpread;

	UPROPERTY(EditAnywhere, Category="Range", BlueprintReadWrite, meta=(AllowPrivateAccess))
	UCurveFloat* HSpread;
};
