// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

USTRUCT()
struct FMyStat : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth;
};

USTRUCT()
struct FMyWeaponStat : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Damage;

};

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMyGameInstance();

	virtual void Init() override;

	FORCEINLINE FMyStat*       GetValue(const int32 Level) const;
	FORCEINLINE FMyWeaponStat* GetWeaponValue(const int32& ID) const;

private:

	UPROPERTY(VisibleAnywhere)
	UDataTable* StatTable;

	UPROPERTY(VisibleAnywhere)
	UDataTable* WeaponStatTable;

};
