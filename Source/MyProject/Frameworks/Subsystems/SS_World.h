// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/Private/Utilities.hpp"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SS_World.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API USS_World : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	USS_World();
	
	template <typename T> requires (std::is_base_of_v<FTableRowBase, T>)
	FORCEINLINE const T* GetRowData(const int32 ID)
	{
		T* Row = nullptr;
		GetValue<T>(ID, &Row);

		if (!Row)
		{
			LOG_FUNC(LogTemp, Error, "Invalid table");
			return nullptr;
		}

		return Row;
	}

	template <typename T> requires (std::is_base_of_v<FTableRowBase, T>)
	void __vectorcall GetValue(const int32 ID, T** const OutData) const
	{
		T* Row = nullptr;

		if constexpr (std::is_same_v<struct FBaseAssetRow, T>)
		{
			Row = AssetTable->FindRow<T>(*FString::FromInt(ID), TEXT(""));
		}

		*OutData = Row;
	}

	FORCEINLINE int32 GetAssetCount() const
	{
		return AssetTable->GetRowMap().Num();
	}

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UDataTable* AssetTable;
};
