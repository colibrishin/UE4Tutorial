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

	template <typename T>
	void __vectorcall GetValue(const int32 ID, T** const OutData) const
	{
		if constexpr (std::is_same_v<struct FMyStat, T>)
		{
			const auto Row = StatTable->FindRow<struct FMyStat>(*FString::FromInt(ID), TEXT(""));
			*OutData = Row;
		}
		if constexpr (std::is_same_v<struct FMyWeaponData, T>)
		{
			const auto Row = WeaponStatTable->FindRow<struct FMyWeaponData>(*FString::FromInt(ID), TEXT(""));
			*OutData = Row;
		}
		if constexpr (std::is_same_v<struct FMyCollectableData, T>)
		{
			const auto Row = CollectableDataTable->FindRow<struct FMyCollectableData>(*FString::FromInt(ID), TEXT(""));
			*OutData = Row;
		}
	}

	FORCEINLINE int32 GetWeaponCount() const
	{
		return WeaponStatTable->GetRowMap().Num();
	}

private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(AllowPrivateAccess))
	UDataTable* StatTable;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(AllowPrivateAccess))
	UDataTable* WeaponStatTable;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(AllowPrivateAccess))
	UDataTable* CollectableDataTable;

};
