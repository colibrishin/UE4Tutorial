// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_Weapon.h"
#include "C_ThrowWeapon.generated.h"


UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_ThrowWeapon : public UC_Weapon
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_ThrowWeapon();

	virtual void Server_StopAttack_Implementation() override;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	float CookTimeCounter;

	UPROPERTY(VisibleAnywhere, Category="Throwable", meta=(AllowPrivateAccess))
	float CookingTime;
	
	UPROPERTY(VisibleAnywhere, Category="Throwable", meta=(AllowPrivateAccess))
	float ThrowForce;

	UPROPERTY(VisibleAnywhere, Category="Throwable", meta=(AllowPrivateAccess))
	float ThrowForceMultiplier;

};
