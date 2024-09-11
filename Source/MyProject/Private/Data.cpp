#include "MyProject/Private/Data.h"

#include "Editor.h"
#include "Utilities.hpp"
#include "MyProject/MyWeaponDataAsset.h"

void FMyCollectableData::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
#if WITH_EDITOR

	const FString StringName = InRowName.ToString();
	if (!StringName.IsNumeric())
	{
		ensure(false);
		LOG_FUNC_PRINTF(LogTemp, Error, "Weapon data row name is not numerical : %s", *InRowName.ToString());
		return;
	}

	if (!CollectableDataAsset)
	{
		return;
	}

	const int32 NewID = FCString::Atoi(*StringName);
	
	// Check whether ID is unique;
	TArray<FMyCollectableData*> Rows;
	InDataTable->GetAllRows(TEXT(""), Rows);
	
	for (int i = 0; i < Rows.Num(); ++i)
	{
		if (Rows[i] &&
			Rows[i]->CollectableDataAsset &&
			Rows[i]->CollectableDataAsset != CollectableDataAsset &&
			Rows[i]->CollectableDataAsset->GetID() == NewID)
		{
			Rows[i]->CollectableDataAsset->SetID(i);
		}

		if (Rows[i]->CollectableDataAsset == CollectableDataAsset)
		{
			ensureAlwaysMsgf(
				InRowName == FString::FromInt(i),
				TEXT("Duplicated data asset found, row %d - row %s"), i, *InRowName.ToString());
		}
	}

	CollectableDataAsset->SetID(NewID);
#endif
}
