// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Private/Data.h"

#include "MyWeaponDataAsset.generated.h"

class AMyWeapon;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	TSubclassOf<AMyWeapon> GetWeaponClass() const { return WeaponBlueprint; }
	const FMyWeaponStat& GetWeaponStat() const { return WeaponStat; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(AllowPrivateAccess))
	TSubclassOf<AMyWeapon> WeaponBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(AllowPrivateAccess))
	FMyWeaponStat WeaponStat;
};
