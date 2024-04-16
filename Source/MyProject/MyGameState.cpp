// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyGameState.h"

#include "MyInGameHUD.h"
#include "MyPlayerState.h"
#include "MyProjectGameModeBase.h"
#include "MyStatComponent.h"

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
		UGameplayStatics::PlaySound2D(this, RoundStartSound);
		const auto& Players = GetWorld()->GetGameState()->PlayerArray;

		for (const auto& Player : Players)
		{
			const auto& MyPlayerState = Cast<AMyPlayerState>(Player);

			if (IsValid(MyPlayerState))
			{
				switch (MyPlayerState->GetTeam())
				{
				case EMyTeam::CT: 
					++AliveCT;
					break;
				case EMyTeam::T:
					++AliveT;
					break;
				case EMyTeam::Unknown:
				default: break;
				}

				const auto& Handle = MyPlayerState->BindOnStateChanged(this, &AMyGameState::HandlePlayerStateChanged);
				PlayerStateDelegateHandles.Add(Player->GetPlayerId(), Handle);
			}
		}
	}
}

void AMyGameState::OnRep_CanBuy() const
{
	OnBuyChanged.Broadcast(bCanBuy);
}

void AMyGameState::HandlePlayerStateChanged(const EMyTeam Team, const EMyCharacterState State)
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
