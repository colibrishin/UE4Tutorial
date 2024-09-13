// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCollectableComponent.h"
#include "../Private/Data.h"
#include "../Private/Enum.h"

#include "Components/ActorComponent.h"

#include "MyWeaponStatComponent.generated.h"

class UMyWeaponDataAsset;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoConsumed , int32 , CurrentAmmoCount , int32 , RemainingCount);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UMyWeaponStatComponent : public UMyCollectableComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMyWeaponStatComponent();
	
	int32         GetDamage() const;
	float         GetRange() const;
	bool          IsHitscan() const;
	EMyWeaponType GetWeaponType() const { return WeaponType; }
	float         GetFireRate() const;
	float         GetCookingTime() const;
	float         GetRadius() const;
	int           GetAmmoPerLoad() const;
	UCurveFloat*  GetVSpread() const;
	UCurveFloat*  GetHSpread() const;

	int32         GetCurrentAmmoCount() const
	{
		return LoadedAmmoCount - AmmoSpent;
	}

	int32         GetRemainingAmmoCount() const
	{
		return (TotalAmmoCount - LoadedAmmoCount > 0) ? TotalAmmoCount - LoadedAmmoCount : 0;
	}

	float GetReloadTime() const;
	int32   GetLoadedAmmoCount() const
	{
		return LoadedAmmoCount;
	}

	bool ConsumeAmmo();
	void Reload();

	FOnAmmoConsumed OnAmmoConsumed;

	virtual void ApplyAsset() const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void UpdateAsset() override;
	
	virtual void InitializeComponent() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	FORCEINLINE const FMyRangeWeaponStat*     GetRangeStat() const;
	FORCEINLINE const FMyMeleeWeaponStat*     GetMeleeStat() const;
	FORCEINLINE const FMyThrowableWeaponStat* GetThrowableStat() const;

	UFUNCTION()
	void OnRep_AmmoConsumed() const;
	
	UPROPERTY(VisibleAnywhere, Category=Stats)
	FString Name;

	UPROPERTY(VisibleAnywhere, Category=Stats)
	int32 Damage;

	UPROPERTY(VisibleAnywhere, Category=Stats)
	EMyWeaponType WeaponType;

	const struct FMyWeaponStatBase* WeaponStat;

};
