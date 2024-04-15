// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyGameState.h"

#include "Net/UnrealNetwork.h"

AMyGameState::AMyGameState()
	: bCanBuy(false)
{
}

void AMyGameState::BuyTimeEnded()
{
	bCanBuy = false;

	if (HasAuthority())
	{
		OnBuyChanged.Broadcast(bCanBuy);
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
}
