// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProjectGameModeBase.h"
#include "Utilities.hpp"

#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRoundProgressChanged, EMyRoundProgress)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBuyChanged, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWinnerSet, EMyTeam)

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
	int32 GetCTCount() const { return AliveCT; }
	int32 GetTCount() const { return AliveT; }

	bool CanBuy() const { return bCanBuy; }
	float GetRoundTime() const
	{
		return (GetServerWorldTimeSeconds() - LastRoundInWorldTime);
	}
	float GetRemainingRoundTime() const
	{
		return AMyProjectGameModeBase::MatchRoundTime - GetRoundTime();
	}

	AMyGameState();

	void HandlePlayerStateChanged(const EMyTeam Team, const EMyCharacterState State);

	void RestartRound();

	virtual void Reset() override;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	void SetRoundProgress(const EMyRoundProgress NewProgress);

	void RecordRoundTime()
	{
		if (HasAuthority())
		{
			LastRoundInWorldTime = GetServerWorldTimeSeconds();
			LOG_FUNC_PRINTF(LogTemp, Warning, "RecordRoundTime: %f", LastRoundInWorldTime);
		}
	}

	void NextRound();

	void OnRoundTimeRanOut();

	void OnFreezeEnded();

	void GoToFreeze();

	void GoToRound();

	void GoToRoundEnd();

	void HandleRoundProgress() const;

	void SetWinner(const EMyTeam NewWinner);

	void BuyTimeEnded();

	UFUNCTION()
	void OnRep_WinnerSet();

	UFUNCTION()
	void OnRep_RoundProgress();

	UFUNCTION()
	void OnRep_CanBuy() const;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_RoundProgress)
	EMyRoundProgress RoundProgress;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_WinnerSet)
	EMyTeam Winner;

	UPROPERTY(VisibleAnywhere, Replicated)
	int32 CTWinCount;

	UPROPERTY(VisibleAnywhere, Replicated)
	int32 TWinCount;

	UPROPERTY(EditAnywhere)
	class USoundWave* CTRoundWinSound;

	UPROPERTY(EditAnywhere)
	class USoundWave* TRoundWinSound;

	UPROPERTY(EditAnywhere)
	class USoundWave* RoundStartSound;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_CanBuy)
	bool bCanBuy;

	UPROPERTY(VisibleAnywhere, Replicated)
	float LastRoundInWorldTime;

	FOnBuyChanged OnBuyChanged;

	FOnWinnerSet OnWinnerSet;

	FTimerHandle BuyTimeHandle;

	FOnRoundProgressChanged OnRoundProgressChanged;

	UPROPERTY(VisibleAnywhere, Replicated)
	int32 AliveCT;

	UPROPERTY(VisibleAnywhere, Replicated)
	int32 AliveT;

	FORCEINLINE void TransitTo
	(
		const EMyRoundProgress            NextProgress,
		void (AMyGameState::*   NextFunction)(),
		const float                       Delay,
		FTimerHandle&                     NextHandle
	);

	FTimerHandle* CurrentHandle;
	FTimerHandle FreezeTimerHandle;
	FTimerHandle RoundTimerHandle;
	FTimerHandle RoundEndTimerHandle;
};
