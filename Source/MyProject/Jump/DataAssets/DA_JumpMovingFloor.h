// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DA_JumpFloor.h"
#include "DA_JumpMovingFloor.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UDA_JumpMovingFloor : public UDA_JumpFloor
{
	GENERATED_BODY()

public:
	float GetLength() const { return Length; }
	FVector GetDirection() const { return Direction; }
	bool IsMoving() const { return bMoving; }
	FRotator GetRotation() const { return Rotation; }
	bool IsRotating() const { return bRotating; }
	UCurveFloat* GetAcceleration() const { return Acceleration; }
	float GetDuration() const { return Duration; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Length;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotating;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;
};
