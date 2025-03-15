// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyPlayerState.h"

#include "AIController.h"

#include "MyGameState.h"
#include "MyPlayerController.h"
#include "Components/C_Buy.h"
#include "Components/C_Health.h"
#include "Components/C_PickUp.h"
#include "Components/Asset/C_CharacterAsset.h"

#include "GameFramework/GameStateBase.h"

#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogPlayerState);

std::mutex AMyPlayerState::TeamAssignMutex;

AMyPlayerState::AMyPlayerState()
	: State(EMyCharacterState::Unknown),
	  Team(EMyTeam::Unknown),
	  Kill(0),
	  Death(0),
	  Assist(0),
	  Money(0)
{
	BuyComponent = CreateDefaultSubobject<UC_Buy>(TEXT("BuyComponent"));
	CharacterAssetID = 4;

	OnPawnSet.AddUniqueDynamic(this, &AMyPlayerState::UpdateCharacterAsset);
}

void AMyPlayerState::BeginPlay()
{
	Super::BeginPlay();

}

void AMyPlayerState::UpdateCharacterAsset(APlayerState* /*InPlayerState*/, APawn* InNewCharacter, APawn* /*InOldCharacter*/)
{
	AA_Character* NewCharacter = Cast<AA_Character>(InNewCharacter);
	
	if (HasAuthority())
	{
		if (NewCharacter)
		{
			if (UC_CharacterAsset* CharacterAsset = NewCharacter->GetComponentByClass<UC_CharacterAsset>())
			{
				CharacterAsset->SetID(CharacterAssetID);
				NewCharacter->FetchAsset();
			}
		}
	}
}

void AMyPlayerState::Reset()
{
	Super::Reset();

	if (HasAuthority())
	{
		LOG_FUNC(LogPlayerState, Log, "Reset PlayerState");

		SetState(EMyCharacterState::Alive);
		if ( const AA_Character* Character = Cast<AA_Character>( GetPawn() ) ) 
		{
			Character->GetHealthComponent()->Reset();
		}
		// todo: Add money by winning or losing
	}
}

void AMyPlayerState::IncrementKills()
{
	if ( HasAuthority() && GetState() == EMyCharacterState::Alive)
	{
		Kill++;
	}
}

void AMyPlayerState::IncrementDeaths()
{
	if ( HasAuthority() && GetState() == EMyCharacterState::Alive )
	{
		Death++;
		SetState( EMyCharacterState::Dead );
	}
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyPlayerState, Team);
	DOREPLIFETIME(AMyPlayerState, Kill);
	DOREPLIFETIME(AMyPlayerState, Death);
	DOREPLIFETIME(AMyPlayerState, Assist);
	DOREPLIFETIME(AMyPlayerState, State);
	DOREPLIFETIME(AMyPlayerState, Money);
	DOREPLIFETIME(AMyPlayerState, BuyComponent);
}

void AMyPlayerState::OnRep_MoneyChanged() const
{
	OnMoneyChanged.Broadcast(Money);
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

void AMyPlayerState::SetState(const EMyCharacterState NewState)
{
	if (HasAuthority())
	{
		State = NewState;
		OnStateChanged.Broadcast(this, State);
	}
}

UC_Buy* AMyPlayerState::GetBuyComponent() const
{
	return BuyComponent;
}

int32 AMyPlayerState::GetCharacterAssetID() const
{
	return CharacterAssetID;
}

void AMyPlayerState::AddMoney(const int32 Amount)
{
	// Server does not participate in the replication.
	if (HasAuthority())
	{
		Money += Amount;
		OnMoneyChanged.Broadcast(Money);
	}
}
