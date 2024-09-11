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
	void SetID(const int32 InID) { ID = InID; }
	
	int32 GetID() const { return ID; }
	USkeletalMesh* GetSkeletalMesh() const { return WeaponSkeletalMesh; }
	UStaticMesh* GetStaticMesh() const { return WeaponStaticMesh; }
	USoundBase* GetFireSound() const { return WeaponFireSound; }
	USoundBase* GetReloadSound() const { return WeaponReloadSound; }
	const FMyWeaponStat& GetWeaponStat() const { return WeaponStat; }

	bool IsSkeletal() const { return WeaponSkeletalMesh != nullptr; }
	bool HasFireSound() const { return WeaponFireSound != nullptr; }
	bool HasReloadSound() const { return WeaponReloadSound != nullptr; }

private:
	UPROPERTY(VisibleAnywhere)
	int32 ID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USkeletalMesh* WeaponSkeletalMesh; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UStaticMesh* WeaponStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USoundBase* WeaponFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USoundBase* WeaponReloadSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(AllowPrivateAccess))
	FMyWeaponStat WeaponStat;
};
