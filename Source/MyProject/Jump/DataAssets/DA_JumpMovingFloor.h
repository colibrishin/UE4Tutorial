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
	bool IsMoving() const { return bMoving; }
	bool IsRotating() const { return bRotating; }
	float GetDuration() const { return Duration; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotating;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;
};
