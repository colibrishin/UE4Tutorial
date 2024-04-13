// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum.h"

#include "Containers/Union.h"

#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"



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

	void __vectorcall GetStatValue(const int32 Level, struct FMyStat** const OutStat) const;
	void __vectorcall GetWeaponValue(const int32 ID, struct FMyWeaponData** const OutData) const;

	FORCEINLINE int32 GetWeaponCount() const
	{
		return WeaponStatTable->GetRowMap().Num();
	}

private:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(AllowPrivateAccess))
	UDataTable* StatTable;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(AllowPrivateAccess))
	UDataTable* WeaponStatTable;

};
