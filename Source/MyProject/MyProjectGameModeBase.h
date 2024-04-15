// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Utilities.hpp"

#include "GameFramework/GameMode.h"
#include "MyProjectGameModeBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnBuyTimeEnded);

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
	static constexpr float MatchBuyTime = 25.f;

	DECL_BINDON(OnBuyTimeEnded)

	bool HasBuyTimeEnded() const { return bHasBuyTimeEnded; }

	AMyProjectGameModeBase();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void HandleMatchHasStarted() override;

	virtual void HandleMatchHasEnded() override;

private:

	void BuyTimeEnded();

	FOnBuyTimeEnded OnBuyTimeEnded;

	FTimerHandle BuyTimeHandle;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bHasBuyTimeEnded;

};
