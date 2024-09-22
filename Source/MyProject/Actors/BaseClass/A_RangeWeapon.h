// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "A_Weapon.h"
#include "A_RangeWeapon.generated.h"

class UC_RangeWeapon;

UCLASS()
class MYPROJECT_API AA_RangeWeapon : public AA_Weapon
{
	GENERATED_BODY()

public:
	static const FName MuzzleSocketName;
	friend class UC_WeaponAsset;
	
	AA_RangeWeapon();

	UC_RangeWeapon* GetRangeWeaponComponent() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_StartBulletTrail();

	void StartBulletTrailImplementation() const;

	virtual void PostFetchAsset() override;

	virtual void Attack() override;

private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UNiagaraComponent* BulletTrailComponent;
	
};
