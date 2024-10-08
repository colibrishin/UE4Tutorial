// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC_Jump.generated.h"

UCLASS(Blueprintable)
class MYPROJECT_API APC_Jump : public APlayerController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APC_Jump();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
