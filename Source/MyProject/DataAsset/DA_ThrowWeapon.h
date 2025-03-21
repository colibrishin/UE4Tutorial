// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DA_Weapon.h"
#include "MyProject/Private/Data.h"
#include "MyProject/Interfaces/EventHandler.h"

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
	float GetEventTimeAfterThrow() const { return EventTimeAfterThrow; }
	const FParameters& GetParameters() const { return Parameters; }
	TSubclassOf<UObject> GetEventHandler() const { return TimeAfterEventHandler; }
	
private:
	UPROPERTY(EditAnywhere, Category="Throwable", BlueprintReadWrite, meta=(AllowPrivateAccess))
	float CookingTime;

	UPROPERTY(EditAnywhere, Category="Throwable", BlueprintReadWrite, meta=(AllowPrivateAccess))
	float ThrowForce;

	UPROPERTY(EditAnywhere, Category="Throwable", BlueprintReadWrite, meta=(AllowPrivateAccess))
	float ThrowForceMultiplier;

	UPROPERTY( EditAnywhere , Category = "Throwable" , BlueprintReadWrite , meta = ( AllowPrivateAccess ) )
	float EventTimeAfterThrow;

	UPROPERTY( EditAnywhere , Category = "Throwable" , BlueprintReadWrite , meta = ( AllowPrivateAccess ) )
	FParameters Parameters;

	UPROPERTY( EditAnywhere , Category = "Throwable" , BlueprintReadWrite , meta = ( MustImplement = "EventHandler", AllowPrivateAccess) )
	TSubclassOf<UObject> TimeAfterEventHandler;
};
