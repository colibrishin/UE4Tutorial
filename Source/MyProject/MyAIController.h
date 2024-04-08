// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMyAIController() = default;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	void MoveRandomly();
	FTimerHandle TimerHandle_MoveRandomly;
	
};
