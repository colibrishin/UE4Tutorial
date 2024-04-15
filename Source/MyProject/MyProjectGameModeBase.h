// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Utilities.hpp"

#include "GameFramework/GameMode.h"
#include "MyProjectGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyProjectGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	AMyProjectGameModeBase();


protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void HandleMatchHasStarted() override;

	virtual void HandleMatchHasEnded() override;

private:

};
