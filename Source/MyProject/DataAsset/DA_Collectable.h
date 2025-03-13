// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DA_AssetBase.h"
#include "DA_Collectable.generated.h"

enum class EMySlotType : uint8;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UDA_Collectable : public UDA_AssetBase
{
	GENERATED_BODY()

public:
	FString GetAssetName() const { return Name; }
	EMySlotType GetSlotType() const { return SlotType; }
	EMultiShapeType GetCollisionType() const { return CollisionType; }
	
private:
	UPROPERTY(EditAnywhere)
	FString Name;
	
	UPROPERTY(EditAnywhere)
	EMySlotType SlotType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	EMultiShapeType CollisionType;
};
