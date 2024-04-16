// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyPlayerState.h"

#include "Utilities.hpp"

#include "GameFramework/GameStateBase.h"

#include "Net/UnrealNetwork.h"

std::mutex AMyPlayerState::TeamAssignMutex;

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

		if (PlayerState->GetTeam() == EMyTeam::CT)
		{
			CTCount++;
		}
		else if (PlayerState->GetTeam() == EMyTeam::T)
		{
			TCount++;
		}
	}

	LOG_FUNC_PRINTF(LogTemp, Warning, "CT: %d, T: %d", CTCount, TCount);

	EMyTeam NewTeam = EMyTeam::Unknown;

	if (CTCount == TCount)
	{
		NewTeam = static_cast<EMyTeam>(FMath::RandRange(1, 2));
		LOG_FUNC_PRINTF(LogTemp, Warning, "Assign team randomly : %d, %s", NewTeam, *EnumToString(NewTeam));
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

	SetTeam(NewTeam);
}
