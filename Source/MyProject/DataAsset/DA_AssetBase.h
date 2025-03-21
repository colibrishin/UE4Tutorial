// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_AssetBase.generated.h"

enum class EMultiShapeType : uint8;
class IAssetFetchable;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UDA_AssetBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	TSubclassOf<AActor> GetOverrideActorClass() const { return OverrideActorClass; }
	USkeletalMesh* GetSkeletalMesh() const { return SkeletalMesh; }
	void        SetID(const int32 InID) { ID = InID; }
	int32       GetID() const { return ID; }
	FVector     GetMeshOffset() const { return MeshOffset; }
	FRotator    GetMeshRotation() const { return MeshRotation; }
	FVector     GetSize() const { return Size; }
	
#if WITH_EDITOR
	DECLARE_MULTICAST_DELEGATE(FOnAssetPropertyChanged);
	FOnAssetPropertyChanged OnAssetPropertyChanged;
	
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
	
private:
	UPROPERTY(VisibleAnywhere)
	int32 ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FVector MeshOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FRotator MeshRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FVector Size;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MustImplement = "AssetFetchable", AllowPrivateAccess ))
	TSubclassOf<AActor> OverrideActorClass;
};
