// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyPlayerState.h"

#include "Utilities.hpp"

#include "GameFramework/GameStateBase.h"

#include "Net/UnrealNetwork.h"

AMyPlayerState::AMyPlayerState()
	: Team(EMyTeam::Unknown),
	  Kill(0),
	  Death(0),
	  Assist(0)
{
}

void AMyPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AssignTeam();
	}
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyPlayerState, Team);
	DOREPLIFETIME(AMyPlayerState, Kill);
	DOREPLIFETIME(AMyPlayerState, Death);
	DOREPLIFETIME(AMyPlayerState, Assist);
}

void AMyPlayerState::AssignTeam()
{
	std::lock_guard<std::mutex> Lock(TeamAssignMutex);

	if (!HasAuthority())
	{
		LOG_FUNC(LogTemp, Error, "This function should be called on server");
		return;
	}

	const auto& Players = GetWorld()->GetGameState()->PlayerArray;

	int32 CTCount = 0;
	int32 TCount = 0;

	for (const auto& Player : Players)
	{
		const auto& PlayerState = Cast<AMyPlayerState>(Player);

		if (PlayerState->Team == EMyTeam::CT)
		{
			CTCount++;
		}
		else if (PlayerState->Team == EMyTeam::T)
		{
			TCount++;
		}
	}

	EMyTeam NewTeam = EMyTeam::Unknown;

	if (CTCount == TCount)
	{
		LOG_FUNC(LogTemp, Warning, "Randomly assign team");
		NewTeam = FMath::RandBool() ? EMyTeam::CT : EMyTeam::T;
	}

	if (CTCount < TCount)
	{
		LOG_FUNC(LogTemp, Warning, "Assign team to CT");
		NewTeam = EMyTeam::CT;
	}
	else if (CTCount > TCount)
	{
		LOG_FUNC(LogTemp, Warning, "Assign team to T");
		NewTeam = EMyTeam::T;
	}

	Team = NewTeam;
}
