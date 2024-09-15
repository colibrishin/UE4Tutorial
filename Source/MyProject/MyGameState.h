// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPlayerState.h"
#include "MyProjectGameModeBase.h"

#include "Private/Utilities.hpp"

#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

class UC_Weapon;
class UC_Buy;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRoundProgressChanged , EMyRoundProgress)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBuyChanged, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWinnerSet, EMyTeam)
DECLARE_MULTICAST_DELEGATE(FOnBombProgressChanging)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBombProgressChanged, EMyBombState)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPlayerStateChanged, class AMyPlayerState*, EMyCharacterState)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAliveCountChanged, EMyTeam, int32)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnNewPlayerJoined, class AMyPlayerState*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBombPicked, class AMyCharacter*)

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
	DECL_BINDON(OnPlayerStateChanged, class AMyPlayerState*, EMyCharacterState)
	DECL_BINDON(OnAliveCountChanged, EMyTeam, int32)
	DECL_BINDON(OnKillOccurred, class AMyPlayerState*, class AMyPlayerState*, const class UC_Weapon*)
	DECL_BINDON(OnNewPlayerJoined, class AMyPlayerState*)
	DECL_BINDON(OnBombPicked, class AMyCharacter*)

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

	EMyBombState GetBombState() const { return BombState; }
	int32        GetCTWins() const { return CTWinCount; }
	int32        GetTWins() const { return TWinCount; }

	AMyGameState();

	void HandleKillOccurred(AMyPlayerState* Killer, AMyPlayerState* Victim, const UC_Weapon* Weapon) const;

	UFUNCTION()
	void HandlePlayerStateChanged(class AMyPlayerState* PlayerState, const EMyCharacterState State);

	void HandleNewPlayer(class AMyPlayerState* State) const;

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

	void HandleOnBombPicked(class AMyCharacter* Character) const;

	UFUNCTION()
	void OnRep_WinnerSet() const;

	UFUNCTION()
	void OnRep_RoundProgress() const;

	UFUNCTION()
	void OnRep_CanBuy() const;

	UFUNCTION()
	void OnRep_BombState(const EMyBombState PreviousBombState);

	UFUNCTION()
	void OnRep_AliveCT() const;

	UFUNCTION()
	void OnRep_AliveT() const;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_KillOccurred(AMyPlayerState* Killer, AMyPlayerState* Victim, const UC_Weapon* Weapon) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_NotifyNewPlayer(AMyPlayerState* State) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_NotifyBombPicked(AA_Character* Character) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ResetBombIndicator();

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

	UPROPERTY(EditAnywhere)
	class USoundWave* BombPlantedSound;

	UPROPERTY(EditAnywhere)
	class USoundWave* BombDefusedSound;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_CanBuy)
	bool bCanBuy;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_BombState)
	EMyBombState BombState;

	UPROPERTY(VisibleAnywhere, Replicated)
	float LastRoundInWorldTime;

	FOnBuyChanged OnBuyChanged;

	FOnWinnerSet OnWinnerSet;

	FOnBombPicked OnBombPicked;

	FTimerHandle BuyTimeHandle;

	FOnRoundProgressChanged OnRoundProgressChanged;

	FOnAliveCountChanged OnAliveCountChanged;

	FOnBombProgressChanged OnBombProgressChanged;

	FOnPlayerStateChanged OnPlayerStateChanged;

	FOnKillOccurred OnKillOccurred;

	FOnNewPlayerJoined OnNewPlayerJoined;

	UPROPERTY(VisibleAnywhere,  ReplicatedUsing=OnRep_AliveCT)
	int32 AliveCT;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AliveT)
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
	FDelegateHandle OnBombPickedHandle;
};
