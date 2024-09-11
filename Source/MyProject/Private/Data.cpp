#include "MyProject/Private/Data.h"

#include "Editor.h"
#include "Utilities.hpp"
#include "MyProject/MyWeaponDataAsset.h"

void FMyWeaponData::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
#if WITH_EDITOR

	const FString StringName = InRowName.ToString();
	if (!StringName.IsNumeric())
	{
		ensure(false);
		LOG_FUNC_PRINTF(LogTemp, Error, "Weapon data row name is not numerical : %s", *InRowName.ToString());
		return;
	}

	if (!WeaponDataAsset)
	{
		return;
	}

	const int32 NewID = FCString::Atoi(*StringName);
	
	// Check whether ID is unique;
	TArray<FMyWeaponData*> Rows;
	InDataTable->GetAllRows(TEXT(""), Rows);
	
	for (int i = 0; i < Rows.Num(); ++i)
	{
		if (Rows[i] &&
			Rows[i]->WeaponDataAsset &&
			Rows[i]->WeaponDataAsset != WeaponDataAsset &&
			Rows[i]->WeaponDataAsset->GetID() == NewID)
		{
			Rows[i]->WeaponDataAsset->SetID(i);
		}

		if (Rows[i]->WeaponDataAsset == WeaponDataAsset)
		{
			ensureAlwaysMsgf(
				InRowName == FString::FromInt(i),
				TEXT("Duplicated data asset found, row %d - row %s"), i, *InRowName.ToString());
		}
	}

	WeaponDataAsset->SetID(NewID);
#endif
}
