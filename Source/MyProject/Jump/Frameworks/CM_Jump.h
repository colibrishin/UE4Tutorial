// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "CM_Jump.generated.h"

UCLASS()
class MYPROJECT_API ACM_Jump : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACM_Jump();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
