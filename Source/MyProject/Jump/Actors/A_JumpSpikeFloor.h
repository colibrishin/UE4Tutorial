// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "A_JumpFloor.h"
#include "A_JumpSpikeFloor.generated.h"

UCLASS()
class MYPROJECT_API AA_JumpSpikeFloor : public AA_JumpFloor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AA_JumpSpikeFloor(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
