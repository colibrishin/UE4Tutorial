// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "A_JumpFloor.h"

#include "GameFramework/Actor.h"
#include "A_JumpMovingFloor.generated.h"

class UC_JumpFloorMovement;

UCLASS()
class MYPROJECT_API AA_JumpMovingFloor : public AA_JumpFloor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AA_JumpMovingFloor(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UC_JumpFloorMovement* FloorMovementComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
