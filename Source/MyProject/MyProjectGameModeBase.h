// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameMode.h"
#include "MyProjectGameModeBase.generated.h"

class APlayerStart;
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

	APlayerStart* GetTSpawnPoint() const { return TSpawnPoint; }
	APlayerStart* GetCTSpawnPoint() const { return CTSpawnPoint; }

protected:
	virtual void InitStartSpot_Implementation(AActor* StartSpot, AController* NewPlayer) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	AActor*      PickPlayerStart(AController* Player) const;

	UPROPERTY()
	APlayerStart* TSpawnPoint;

	UPROPERTY()
	APlayerStart* CTSpawnPoint;
};
