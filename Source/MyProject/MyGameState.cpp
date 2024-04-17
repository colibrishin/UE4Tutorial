// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyGameState.h"

#include "MyCharacter.h"
#include "MyInGameHUD.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "MyProjectGameModeBase.h"
#include "MyStatComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Net/UnrealNetwork.h"

AMyGameState::AMyGameState()
	: RoundProgress(EMyRoundProgress::Unknown),
	  bCanBuy(false),
	  AliveCT(0),
	  AliveT(0)
{
	static ConstructorHelpers::FObjectFinder<USoundWave> RoundStartSoundFinder
		(TEXT("SoundWave'/Game/Models/sounds/moveout.moveout'"));
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

void AMyGameState::OnRep_RoundProgress()
{
	if (RoundProgress == EMyRoundProgress::Playing)
	{
		UGameplayStatics::PlaySound2D(this , RoundStartSound);
	}

	OnRoundProgressChanged.Broadcast(RoundProgress);
}

void AMyGameState::OnRep_CanBuy() const
{
	OnBuyChanged.Broadcast(bCanBuy);
}

void AMyGameState::HandlePlayerStateChanged(const EMyTeam Team, const EMyCharacterState State)
{
	if (HasAuthority())
	{
		switch (State)
		{
		case EMyCharacterState::Alive:
			switch (Team)
			{
			case EMyTeam::CT: ++AliveCT;
				break;
			case EMyTeam::T: ++AliveT;
				break;
			case EMyTeam::Unknown: break;
			}
			break;
		case EMyCharacterState::Planting: 
			break;
		case EMyCharacterState::Defusing: 
			break;
		case EMyCharacterState::Dead:
			switch (Team)
			{
				case EMyTeam::CT: --AliveCT;
				break;
				case EMyTeam::T: --AliveT;
				break;
				case EMyTeam::Unknown: break;
			}
			break;
		case EMyCharacterState::Unknown:
		default: break;
		}
	}
}

void AMyGameState::SetRoundProgress(const EMyRoundProgress NewProgress)
{
	if (HasAuthority())
	{
		RoundProgress = NewProgress;

		OnRoundProgressChanged.Broadcast(RoundProgress);
	}
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

	//SetLifeSpan(AMyProjectGameModeBase::MatchRoundTime);
}

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyGameState, bCanBuy);
	DOREPLIFETIME(AMyGameState, RoundProgress);
	DOREPLIFETIME(AMyGameState, AliveCT);
	DOREPLIFETIME(AMyGameState, AliveT);
}
