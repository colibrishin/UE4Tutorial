#pragma once

#include "Engine/DataTable.h"

#include "UObject/ObjectMacros.h"

#include "Windows/WindowsPlatform.h"

#include "Data.generated.h"

class UDA_AssetBase;
class UDA_Collectable;

USTRUCT()
struct FBaseAssetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AssetBundles))
	UDA_AssetBase* AssetToLink;

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
};
