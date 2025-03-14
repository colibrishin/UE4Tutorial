#pragma once

#include "Engine/DataTable.h"

#include "UObject/ObjectMacros.h"

#include "Windows/WindowsPlatform.h"

#include "Data.generated.h"

class UDA_AssetBase;
class UDA_Collectable;

USTRUCT(BlueprintType)
struct FParameters
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere , Category = "Values" , BlueprintReadWrite , meta = ( AllowPrivateAccess ) )
	TMap<FString , float> FloatParameters;

	UPROPERTY( EditAnywhere , Category = "Values" , BlueprintReadWrite , meta = ( AllowPrivateAccess ) )
	TMap<FString , int> IntegerParameters;
	
	UPROPERTY( EditAnywhere , Category = "Values" , BlueprintReadWrite , meta = ( AllowPrivateAccess ) )
	TMap<FString , FVector> VectorParameters;
};

USTRUCT()
struct FBaseAssetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AssetBundles))
	UDA_AssetBase* AssetToLink;

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
};
