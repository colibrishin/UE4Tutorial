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
DECLARE_MULTICAST_DELEGATE(FOnBombProgressChanging)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBombProgressChanged, EMyBombState)

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
	DECL_BINDON(OnWinnerSet, EMyTeam)
	DECL_BINDON(OnBombProgressChanged, EMyBombState)

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

	class AMyC4* GetC4() const { return RoundC4; }
	EMyBombState          GetBombState() const { return BombState; }

	AMyGameState();

	void HandlePlayerStateChanged(class AMyPlayerController* PlayerController, const EMyTeam Team, const EMyCharacterState State);

	void RestartRound();

	virtual void Reset() override;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void OnBombStateChanged(const EMyBombState NewState);

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

	UFUNCTION()
	void OnRep_BombState();

	UPROPERTY(VisibleAnywhere)
	TSubclassOf<class AMyC4> C4BluePrint;

	UPROPERTY(VisibleAnywhere, Replicated)
	class AMyC4* RoundC4;

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

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_BombState)
	EMyBombState BombState;

	UPROPERTY(VisibleAnywhere, Replicated)
	float LastRoundInWorldTime;

	FOnBuyChanged OnBuyChanged;

	FOnWinnerSet OnWinnerSet;

	FTimerHandle BuyTimeHandle;

	FOnRoundProgressChanged OnRoundProgressChanged;

	FOnBombProgressChanged OnBombProgressChanged;

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

	FDelegateHandle OnBombStateChangedHandle;
};
