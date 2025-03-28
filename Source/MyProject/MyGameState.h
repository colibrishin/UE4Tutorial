// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Public/CommonDelegate.h"
#include "MyProject/Private/Utilities.hpp"

#include "MyGameState.generated.h"

enum class EMyTeam : uint8;
enum class EMyRoundProgress : uint8;
enum class EMyBombState : uint8;
class AA_C4;
class AA_Character;
class UC_Weapon;
class UC_Buy;
class AMyPlayerState;
class AMyPlayerController;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRoundProgressChanged , EMyRoundProgress)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuyChanged, bool, InFlag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWinnerSet, const EMyTeam, InTeam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBombProgressChanged, EMyBombState, InBombState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerStateChanged, AMyPlayerState*, InPlayerState, EMyCharacterState, InCharacterState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAliveCountChanged, EMyTeam, InTeam, const int32, InCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewPlayerJoined, AMyPlayerState*, InNewPlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBombPicked, AA_Character*, InCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnBombStateChangedDynamic , const EMyBombState , InOldBombState, const EMyBombState , InNewBombState  , const AA_Character* , InPlanter , const AA_Character* , InDefuser);

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyGameState : public AGameStateBase
{
	GENERATED_BODY()

	friend class AMyPlayerController;
public:

	FOnBuyChanged OnBuyChanged;
	
	FOnWinnerSet OnWinnerSet;
	
	FTimerHandle BuyTimeHandle;
	
	FOnRoundProgressChanged OnRoundProgressChanged;
	
	FOnAliveCountChanged OnAliveCountChanged;
	
	FOnPlayerStateChanged OnPlayerStateChanged;
	
	FOnKillOccurred OnKillOccurred;
	
	FOnNewPlayerJoined OnNewPlayerJoined;

	// Forwarding C4 delegate instead of using c4's raw delegate, due to the fact that the c4 can be destroyed;
	FOnBombStateChangedDynamic OnBombStateChanged;

	FOnBombPicked OnBombPicked;

	AA_C4* GetC4() const { return RoundC4; }
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
		return MatchRoundTime - GetRoundTime();
	}
	float GetStartTickInServerTime() const 
	{
		return StartTickInServerTime;
	}

	int32        GetCTWins() const { return CTWinCount; }
	int32        GetTWins() const { return TWinCount; }

	AMyGameState();

	UFUNCTION()
	void HandleKillOccurred( AMyPlayerController* Killer, AMyPlayerController* Victim, UC_PickUp* Weapon);

	UFUNCTION()
	void HandlePlayerStateChanged(AMyPlayerState* PlayerState, const EMyCharacterState State);

	UFUNCTION()
	void HandleNewPlayer(AMyPlayerState* State) const;

	void RestartRound();

	virtual void Reset() override;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick( const float DeltaTime ) override;

private:
	UFUNCTION()
	void HandleBombStateChanged( const EMyBombState InOldState , const EMyBombState InNewState , const AA_Character* InPlanter , const AA_Character* InDefuser );

	UFUNCTION()
	void UpdateC4( AActor* InNewActor );

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

	UFUNCTION()
	void OnRep_C4( AA_C4* PreviousC4 );

	UFUNCTION()
	void BuyTimeEnded();

	UFUNCTION()
	void HandleOnBombPicked( AActor* InC4Actor );

	UFUNCTION()
	void OnRep_WinnerSet() const;

	UFUNCTION()
	void OnRep_RoundProgress() const;

	UFUNCTION()
	void OnRep_CanBuy() const;

	UFUNCTION()
	void OnRep_AliveCT() const;

	UFUNCTION()
	void OnRep_AliveT() const;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_NotifyNewPlayer(AMyPlayerState* State) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_NotifyBombPicked(AA_Character* Character) const;

	UPROPERTY(EditAnywhere)
	USoundWave* CTRoundWinSound;

	UPROPERTY(EditAnywhere)
	USoundWave* TRoundWinSound;

	UPROPERTY(EditAnywhere)
	USoundWave* RoundStartSound;

	UPROPERTY(EditAnywhere)
	USoundWave* BombPlantedSound;

	UPROPERTY(EditAnywhere)
	USoundWave* BombDefusedSound;

	UPROPERTY(VisibleAnywhere, Replicated, meta=(AllowPrivateAccess))
	float StartTickInServerTime;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	float MatchRoundTime;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	float MatchBuyTime;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_RoundProgress)
	EMyRoundProgress RoundProgress;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_WinnerSet)
	EMyTeam Winner;

	UPROPERTY(VisibleAnywhere, Replicated)
	int32 CTWinCount;

	UPROPERTY(VisibleAnywhere, Replicated)
	int32 TWinCount;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_CanBuy)
	bool bCanBuy;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_C4)
	AA_C4* RoundC4;

	UPROPERTY(VisibleAnywhere, Replicated)
	float LastRoundInWorldTime;

	UPROPERTY(VisibleAnywhere,  ReplicatedUsing=OnRep_AliveCT)
	int32 AliveCT;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AliveT)
	int32 AliveT;

	void TransitTo
	(
		const EMyRoundProgress            NextProgress,
		void (AMyGameState::*   NextFunction)(),
		const float                       Delay,
		FTimerHandle&                     NextHandle
	);

	FDelegateHandle C4DelegateHandle;
	FTimerHandle* CurrentHandle;
	FTimerHandle FreezeTimerHandle;
	FTimerHandle RoundTimerHandle;
	FTimerHandle RoundEndTimerHandle;
};
