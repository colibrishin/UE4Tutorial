// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_Asset.h"
#include "MyProject/Private/Enum.h"

#include "C_CollectableAsset.generated.h"

UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_CollectableAsset : public UC_Asset
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_CollectableAsset();

	EMySlotType GetSlot() const;

	FString GetAssetName() const;
	
	virtual void ApplyAsset() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};