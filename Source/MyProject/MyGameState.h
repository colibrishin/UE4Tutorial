// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utilities.hpp"

#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBuyChanged, bool)

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyGameState : public AGameState
{
	GENERATED_BODY()

public:
	static constexpr float MatchStartDelay = 5.f;
	static constexpr float MatchRoundTime = 300.f;
	static constexpr float MatchBuyTime = 25.f;

	DECL_BINDON(OnBuyChanged, bool)

	AMyGameState();

	bool CanBuy() const { return bCanBuy; }

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void BuyTimeEnded();

	UFUNCTION()
	void OnRep_CanBuy() const;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_CanBuy)
	bool bCanBuy;

	FOnBuyChanged OnBuyChanged;

	FTimerHandle BuyTimeHandle;
};
