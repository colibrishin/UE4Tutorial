// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DA_Weapon.h"

#include "DA_ThrowWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UDA_ThrowWeapon : public UDA_Weapon
{
	GENERATED_BODY()

public:
	float GetCookingTime() const { return CookingTime; }
	float GetThrowForce() const { return ThrowForce; }
	float GetThrowMultiplier() const { return ThrowForceMultiplier; }
	
private:
	UPROPERTY(EditAnywhere, Category="Throwable", BlueprintReadWrite, meta=(AllowPrivateAccess))
	float CookingTime;

	UPROPERTY(EditAnywhere, Category="Throwable", BlueprintReadWrite, meta=(AllowPrivateAccess))
	float ThrowForce;

	UPROPERTY(EditAnywhere, Category="Throwable", BlueprintReadWrite, meta=(AllowPrivateAccess))
	float ThrowForceMultiplier;
};
