// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GS_Jump.generated.h"

class UC_JumpCheckpoint;

UCLASS(Blueprintable)
class MYPROJECT_API AGS_Jump : public AGameStateBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGS_Jump();

	void SetLastCheckPoint(UC_JumpCheckpoint* InCheckPoint) { LastCheckpoint = InCheckPoint; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UC_JumpCheckpoint* LastCheckpoint;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
