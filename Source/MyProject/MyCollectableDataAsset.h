// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/MyCollectableComponent.h"

#include "MyCollectableDataAsset.generated.h"

enum class EMySlotType : uint8;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyCollectableDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	USkeletalMesh* GetSkeletalMesh() const { return SkeletalMesh; }
	UStaticMesh* GetStaticMesh() const { return StaticMesh; }
	bool IsSkeletal() const { return SkeletalMesh != nullptr; }

	void        SetID(const int32 InID) { ID = InID; }
	int32       GetID() const { return ID; }
	EMySlotType GetSlotType() const { return SlotType; }

private:
	UPROPERTY(VisibleAnywhere)
	int32 ID;
	
	UPROPERTY(EditAnywhere)
	EMySlotType SlotType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
    USkeletalMesh* SkeletalMesh; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
    UStaticMesh* StaticMesh;
};
