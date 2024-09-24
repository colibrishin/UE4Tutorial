// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_CollectableAsset.h"
#include "C_WeaponAsset.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWeaponAssetComponent, Log, All);

UCLASS(ClassGroup=(Custom) , meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UC_WeaponAsset : public UC_CollectableAsset
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_WeaponAsset();

	virtual void ApplyAsset() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
};
