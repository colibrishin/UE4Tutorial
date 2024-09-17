// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetFetchable.h"

#include "MyProject/Components/Asset/C_CollectableAsset.h"

// Add default functionality here for any IAssetFetchable functions that are not pure virtual.
void IAssetFetchable::FetchAsset()
{
	const AActor* Object = Cast<AActor>(this);
	ensureAlwaysMsgf(Object, TEXT("IAssetFetchable is considered AActor as base class"));
	
	UC_Asset* AssetComponent = Object->GetComponentByClass<UC_Asset>();
	ensureAlwaysMsgf(AssetComponent, TEXT("AssetComponent is not found"));
	AssetComponent->ApplyAsset();
}
