// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyGameState.h"

#include "MyProjectGameModeBase.h"

#include "Net/UnrealNetwork.h"

AMyGameState::AMyGameState()
	: bCanBuy(false),
	  RoundProgress(EMyRoundProgress::Unknown)
{
	static ConstructorHelpers::FObjectFinder<USoundWave> RoundStartSoundFinder(TEXT("SoundWave'/Game/Models/sounds/moveout.moveout'"));
	RoundStartSound = RoundStartSoundFinder.Object;
}

void AMyGameState::BuyTimeEnded()
{
	bCanBuy = false;

	if (HasAuthority())
	{
		OnBuyChanged.Broadcast(bCanBuy);
	}
}

void AMyGameState::OnRep_RoundProgress() const
{
	if (RoundProgress == EMyRoundProgress::Playing)
	{
		UGameplayStatics::PlaySound2D(this, RoundStartSound);
	}
}

void AMyGameState::OnRep_CanBuy() const
{
	OnBuyChanged.Broadcast(bCanBuy);
}

void AMyGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		bCanBuy = true;

		GetWorldTimerManager().SetTimer
		(
			BuyTimeHandle, 
			this, 
			&AMyGameState::BuyTimeEnded, 
			MatchBuyTime, 
			false
		);
	}
}

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyGameState, bCanBuy);
	DOREPLIFETIME(AMyGameState, RoundProgress);
}
