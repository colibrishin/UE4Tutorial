#include "MyProject/Private/Data.h"
#include "Utilities.hpp"
#include "MyProject/DataAsset/DA_AssetBase.h"

void FBaseAssetRow::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
#if WITH_EDITOR
	if (GIsEditor)
	{
		const FString StringName = InRowName.ToString();
		if (!StringName.IsNumeric())
		{
			ensure(false);
			LOG_FUNC_PRINTF(LogTemp, Error, "Data row name is not numerical : %s", *InRowName.ToString());
			return;
		}

		if (!AssetToLink)
		{
			return;
		}

		const int32 NewID = FCString::Atoi(*StringName);
	
		// Check whether ID is unique;
		TArray<FBaseAssetRow*> Rows;
		InDataTable->GetAllRows(TEXT(""), Rows);
	
		for (int i = 0; i < Rows.Num(); ++i)
		{
			if (Rows[i] &&
				Rows[i]->AssetToLink &&
				Rows[i]->AssetToLink != AssetToLink &&
				Rows[i]->AssetToLink->GetID() == NewID)
			{
				Rows[i]->AssetToLink->SetID(i);
			}

			if (Rows[i]->AssetToLink == AssetToLink)
			{
				ensureAlwaysMsgf(
					InRowName == FString::FromInt(i),
					TEXT("Duplicated data asset found, row %d - row %s"), i, *InRowName.ToString());
			}
		}

		AssetToLink->SetID(NewID);
	}
#endif
}
