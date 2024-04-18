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
class MYPROJECT_API AMyProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	static constexpr float MatchFreezeTime = 5.f;
	static constexpr float MatchRoundTime = 300.f;
	static constexpr float MatchRoundEndDelay = 5.f;

	AMyProjectGameModeBase();

	virtual void RestartPlayer(AController* NewPlayer) override;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	AActor*      PickPlayerStart(AController* Player) const;

};
