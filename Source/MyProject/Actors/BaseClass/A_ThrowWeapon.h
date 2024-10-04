// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "A_Weapon.h"
#include "A_ThrowWeapon.generated.h"

class UProjectileMovementComponent;

UCLASS()
class MYPROJECT_API AA_ThrowWeapon : public AA_Weapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AA_ThrowWeapon(const FObjectInitializer& ObjectInitializer);

	UProjectileMovementComponent* GetProjectileMovementComponent() const { return ProjectileMovementComponent; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	UProjectileMovementComponent* ProjectileMovementComponent;
};
