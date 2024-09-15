// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_AssetBase.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UDA_AssetBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	USkeletalMesh* GetSkeletalMesh() const { return SkeletalMesh; }
	void        SetID(const int32 InID) { ID = InID; }
	int32       GetID() const { return ID; }

private:
	UPROPERTY(VisibleAnywhere)
	int32 ID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USkeletalMesh* SkeletalMesh; 
};
