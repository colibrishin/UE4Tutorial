// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "C_Asset.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAssetComponent, Log, All);

class UDA_AssetBase;

UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_Asset : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_Asset();

	void SetID(const uint32 InID)
	{
		if (GetNetMode() != NM_Client)
		{
			ID = InID;
			FetchAsset();
		}
	}
	
	uint32 GetID() const { return ID; }

	template <typename T> requires (std::is_base_of_v<UDA_AssetBase, T>)
	T* GetAsset() const
	{
		return Cast<T>(AssetData);
	}

	virtual void ApplyAsset();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_ID();

	void FetchAsset();

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_ID, meta=(AllowPrivateAccess))
	uint32 ID;

	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	UDA_AssetBase* AssetData;
	
};
