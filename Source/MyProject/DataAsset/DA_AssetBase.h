// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_AssetBase.generated.h"

class IAssetFetchable;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UDA_AssetBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	TSubclassOf<IAssetFetchable> GetOverrideActorClass() const { return OverrideActorClass; }
	USkeletalMesh* GetSkeletalMesh() const { return SkeletalMesh; }
	void        SetID(const int32 InID) { ID = InID; }
	int32       GetID() const { return ID; }
	FVector     GetMeshOffset() const { return MeshOffset; }
	FVector     GetSize() const { return Size; }

private:
	UPROPERTY(VisibleAnywhere)
	int32 ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FVector MeshOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FVector Size;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TSubclassOf<IAssetFetchable> OverrideActorClass;
};
