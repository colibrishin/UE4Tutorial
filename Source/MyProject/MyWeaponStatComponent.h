// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum.h"

#include "Components/ActorComponent.h"

#include "MyWeaponStatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UMyWeaponStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMyWeaponStatComponent();

	int32         GetID() const { return ID; }
	int32         GetDamage() const;
	float         GetRange() const;
	bool          IsHitscan() const;
	EMyWeaponType GetWeaponType() const { return WeaponType; }
	float         GetFireRate() const;

	int32         GetCurrentAmmoCount() const
	{
		return LoadedAmmoCount - AmmoSpent;
	}

	int32         GetRemainingAmmoCount() const
	{
		return (TotalAmmoCount - LoadedAmmoCount > 0) ? TotalAmmoCount - LoadedAmmoCount : 0;
	}

	bool  ConsumeAmmo();
	void  Reload();
	float GetReloadTime() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

private:
	FORCEINLINE const struct FMyRangeWeaponStat* GetRangeStat() const;
	FORCEINLINE const struct FMyMeleeWeaponStat* GetMeleeStat() const;

	UPROPERTY(EditAnywhere, Category=Stats)
	int32 ID;

	UPROPERTY(VisibleAnywhere, Category=Stats)
	FString Name;

	UPROPERTY(VisibleAnywhere, Category=Stats)
	int32 Damage;

	UPROPERTY(VisibleAnywhere, Category=Stats)
	int32 AmmoSpent;

	UPROPERTY(VisibleAnywhere, Category=Stats)
	int32 LoadedAmmoCount;

	UPROPERTY(VisibleAnywhere, Category=Stats)
	int32 AmmoPerLoad;

	UPROPERTY(VisibleAnywhere, Category=Stats)
	int32 TotalAmmoCount;

	UPROPERTY(VisibleAnywhere, Category=Stats)
	TEnumAsByte<EMyWeaponType> WeaponType;

	const struct FMyWeaponStatBase* WeaponStat;

};
