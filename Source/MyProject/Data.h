#pragma once
#include "Enum.h"

#include "Engine/DataTable.h"

#include "UObject/ObjectMacros.h"

#include "Windows/WindowsPlatform.h"

#include "Data.generated.h"

USTRUCT()
struct FMyStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth;
};

USTRUCT()
struct FMyWeaponStatBase : public FTableRowBase
{
	GENERATED_BODY()
};

USTRUCT()
struct FMyRangeWeaponStat : public FMyWeaponStatBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Magazine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReloadTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsHitscan;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsAimable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range;
};

USTRUCT()
struct FMyMeleeWeaponStat : public FMyWeaponStatBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;

};

USTRUCT()
struct FMyThrowableWeaponStat : public FMyWeaponStatBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CookingTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThrowForce;

	// todo: can be replaced with a mass
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThrowForceMultiplier;

};

USTRUCT()
struct FMyCollectableData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMySlotType SlotType;

};

USTRUCT()
struct FMyWeaponStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMyWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Price;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(AllowPrivateAccess))
	FMyMeleeWeaponStat MeleeWeaponStat{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(AllowPrivateAccess))
	FMyRangeWeaponStat RangeWeaponStat{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(AllowPrivateAccess))
	FMyThrowableWeaponStat ThrowableWeaponStat{};

public:
	template <typename T, typename BaseLock = std::enable_if_t<std::is_base_of_v<FMyWeaponStatBase, T>>>
	FORCEINLINE const T* Get() const
	{
		if constexpr (std::is_same_v<T, FMyMeleeWeaponStat>)
		{
			return &MeleeWeaponStat;
		}
		if constexpr (std::is_same_v<T, FMyRangeWeaponStat>)
		{
			return &RangeWeaponStat;
		}
		if constexpr (std::is_same_v<T, FMyThrowableWeaponStat>)
		{
			return &ThrowableWeaponStat;
		}

		return nullptr;
	}
};

USTRUCT()
struct FMyWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMyWeaponDataAsset* WeaponDataAsset;
};