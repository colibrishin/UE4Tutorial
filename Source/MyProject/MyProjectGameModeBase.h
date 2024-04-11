// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

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
	static constexpr float MatchStartDelay = 5.f;
	static constexpr float MatchRoundTime = 300.f;

	AMyProjectGameModeBase();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void HandleMatchHasStarted() override;

	virtual void HandleMatchHasEnded() override;

};
