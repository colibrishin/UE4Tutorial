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
	int32 GetMaxHealth() const { return MaxHealth; }
	USkeletalMesh* GetArmMesh() const { return ArmMesh; }
	TSubclassOf<UAnimInstance> GetAnimInstance() const { return AnimInstance; }
	TSubclassOf<UAnimInstance> GetArmAnimInstance() const { return ArmAnimInstance; }
	FVector GetArmOffset() const { return ArmOffset; }
	FRotator GetArmRotation() const { return ArmRotation; }
	FVector GetCameraOffset() const { return CameraOffset; }
	FRotator GetCameraRotation() const { return CameraRotation; }

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
};
