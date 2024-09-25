// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DA_AssetBase.h"
#include "DA_Character.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UDA_Character : public UDA_AssetBase
{
	GENERATED_BODY()

public:
	int32                      GetMaxHealth() const { return MaxHealth; }
	USkeletalMesh*             GetArmMesh() const { return ArmMesh; }
	TSubclassOf<UAnimInstance> GetAnimInstance() const { return AnimInstance; }
	TSubclassOf<UAnimInstance> GetArmAnimInstance() const { return ArmAnimInstance; }
	FVector                    GetArmOffset() const { return ArmOffset; }
	FRotator                   GetArmRotation() const { return ArmRotation; }
	FVector                    GetCameraOffset() const { return CameraOffset; }
	FRotator                   GetCameraRotation() const { return CameraRotation; }
	FVector                    GetCameraArmTargetOffset() const { return CameraArmTargetOffset; }
	FVector                    GetCameraArmSocketOffset() const { return CameraArmSocketOffset; }
	float                      GetCameraArmLength() const { return CameraArmLength; }
	float                      GetCollisionProbeSize() const { return CollisionProbeSize; }
	bool                       GetOwnerNoSee() const { return bOwnerNoSee; }
	bool                       GetMeshOverlapEvent() const { return bMeshOverlapEvent; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* ArmMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> ArmAnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ArmOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator ArmRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CameraOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator CameraRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	float CameraArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CameraArmSocketOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CameraArmTargetOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CollisionProbeSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOwnerNoSee;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMeshOverlapEvent;
};
