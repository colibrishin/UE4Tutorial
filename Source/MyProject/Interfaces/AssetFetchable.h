// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AssetFetchable.generated.h"

class UC_Asset;
class UC_CollectableAsset;
// This class does not need to be modified.
UINTERFACE()
class UAssetFetchable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYPROJECT_API IAssetFetchable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	template <typename T = UC_Asset> requires (std::is_base_of_v<UC_Asset, T>)
	void FetchAsset()
	{
		const AActor* Object = Cast<AActor>(this);
		ensureAlwaysMsgf(Object, TEXT("IAssetFetchable is considered AActor as base class"));
	
		T* AssetComponent = Object->GetComponentByClass<T>();
		ensureAlwaysMsgf(AssetComponent, TEXT("AssetComponent is not found"));
		AssetComponent->FetchAsset();
		AssetComponent->ApplyAsset();
		PostFetchAsset();
	}

protected:
	virtual void PostFetchAsset();
	
};
