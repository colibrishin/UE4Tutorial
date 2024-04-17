// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utilities.hpp"

#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRoundProgressChanged, EMyRoundProgress)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBuyChanged, bool)

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	static constexpr float MatchBuyTime = 25.f;

	DECL_BINDON(OnBuyChanged, bool)
	DECL_BINDON(OnRoundProgressChanged, EMyRoundProgress)
	EMyRoundProgress GetState() const { return RoundProgress; }

	AMyGameState();

	bool CanBuy() const { return bCanBuy; }
	void SetRoundProgress(const EMyRoundProgress NewProgress);

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void BuyTimeEnded();

	UFUNCTION()
	void OnRep_RoundProgress();

	UFUNCTION()
	void OnRep_CanBuy() const;

	void HandlePlayerStateChanged(const EMyTeam Team, const EMyCharacterState State);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_RoundProgress)
	EMyRoundProgress RoundProgress;

	UPROPERTY(EditAnywhere)
	class USoundWave* RoundStartSound;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_CanBuy)
	bool bCanBuy;

	FOnBuyChanged OnBuyChanged;

	FTimerHandle BuyTimeHandle;

	FOnRoundProgressChanged OnRoundProgressChanged;

	UPROPERTY(VisibleAnywhere, Replicated)
	int32 AliveCT;

	UPROPERTY(VisibleAnywhere, Replicated)
	int32 AliveT;

	TMap<int32, FDelegateHandle> PlayerStateDelegateHandles;
};
