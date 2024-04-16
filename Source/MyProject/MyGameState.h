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
class MYPROJECT_API AMyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	static constexpr float MatchBuyTime = 25.f;

	DECL_BINDON(OnBuyChanged, bool)

	AMyGameState();

	bool CanBuy() const { return bCanBuy; }
	void SetRoundProgress(const EMyRoundProgress NewProgress)
	{
		if (HasAuthority())
		{
			RoundProgress = NewProgress;
		}
	}

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(EditAnywhere)
	class USoundWave* RoundStartSound;

	void BuyTimeEnded();

	UFUNCTION()
	void OnRep_RoundProgress() const;

	UFUNCTION()
	void OnRep_CanBuy() const;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_CanBuy)
	bool bCanBuy;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_RoundProgress)
	EMyRoundProgress RoundProgress;

	FOnBuyChanged OnBuyChanged;

	FTimerHandle BuyTimeHandle;
};
