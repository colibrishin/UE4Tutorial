// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data.h"

#include "MyWeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	TSubclassOf<class AMyWeapon> GetWeaponClass() const { return WeaponBlueprint->GetBlueprintClass(); }
	const FMyWeaponStat& GetWeaponStat() const { return WeaponStat; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(AllowPrivateAccess))
	UBlueprint* WeaponBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(AllowPrivateAccess))
	FMyWeaponStat WeaponStat;
};
