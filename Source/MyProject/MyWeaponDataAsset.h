// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCollectableDataAsset.h"
#include "Private/Data.h"

#include "MyWeaponDataAsset.generated.h"

class AMyWeapon;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyWeaponDataAsset : public UMyCollectableDataAsset
{
	GENERATED_BODY()

public:
	USoundBase* GetFireSound() const { return WeaponFireSound; }
	USoundBase* GetReloadSound() const { return WeaponReloadSound; }
	const FMyWeaponStat& GetWeaponStat() const { return WeaponStat; }

	bool HasFireSound() const { return WeaponFireSound != nullptr; }
	bool HasReloadSound() const { return WeaponReloadSound != nullptr; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USoundBase* WeaponFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USoundBase* WeaponReloadSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(AllowPrivateAccess))
	FMyWeaponStat WeaponStat;
};
