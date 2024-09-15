// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DA_Collectable.h"
#include "DA_Weapon.generated.h"

enum class EMyWeaponType : uint8;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UDA_Weapon : public UDA_Collectable
{
	GENERATED_BODY()

public:
	EMyWeaponType GetType() const { return Type; }
	USoundBase* GetAttackSound() const { return WeaponFireSound; }
	USoundBase* GetReloadSound() const { return WeaponReloadSound; }
	int32 GetDamage() const { return Damage; }
	int32 GetPrice() const { return Price; }
	float GetRadius() const { return Radius; }
	int32 GetMaxAmmo() const { return MaxAmmo; }
	int32 GetMagazine() const { return Magazine; }
	float GetReloadTime() const { return ReloadTime; }
	float GetAttackRate() const { return AttackRate; }
	bool GetSpray() const { return bSpray; }
	float GetRange() const { return Range; }
	
private:
	UPROPERTY(EditAnywhere, Category="Common", BlueprintReadWrite, meta=(AllowPrivateAccess))
	EMyWeaponType Type;

	UPROPERTY(EditAnywhere, Category="Common", BlueprintReadWrite, meta=(AllowPrivateAccess))
	USoundBase* WeaponFireSound;

	UPROPERTY(EditAnywhere, Category="Common", BlueprintReadWrite, meta=(AllowPrivateAccess))
	USoundBase* WeaponReloadSound;

	UPROPERTY(EditAnywhere, Category="Common", BlueprintReadWrite, meta=(AllowPrivateAccess))
	int32 Damage;

	UPROPERTY(EditAnywhere, Category="Common", BlueprintReadWrite, meta=(AllowPrivateAccess))
	int32 Price;

	UPROPERTY(EditAnywhere, Category="Common", BlueprintReadWrite, meta=(AllowPrivateAccess))
	float Radius;

	UPROPERTY(EditAnywhere, Category="Common", BlueprintReadWrite, meta=(AllowPrivateAccess))
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, Category="Common", BlueprintReadWrite, meta=(AllowPrivateAccess))
	int32 Magazine;

	UPROPERTY(EditAnywhere, Category="Common", BlueprintReadWrite, meta=(AllowPrivateAccess))
	float ReloadTime;

	UPROPERTY(EditAnywhere, Category="Common", BlueprintReadWrite, meta=(AllowPrivateAccess))
	float AttackRate;

	UPROPERTY(EditAnywhere, Category="Common", BlueprintReadWrite, meta=(AllowPrivateAccess))
	bool bSpray;

	UPROPERTY(EditAnywhere, Category="Common", BlueprintReadWrite, meta=(AllowPrivateAccess))
	float Range;
};
