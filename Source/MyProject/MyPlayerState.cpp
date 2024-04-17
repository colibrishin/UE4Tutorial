// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyPlayerState.h"

#include "CommonRoundProgress.hpp"
#include "Utilities.hpp"

#include "GameFramework/GameStateBase.h"
#include "MyCharacter.h"
#include "MyGameState.h"
#include "MyInGameHUD.h"
#include "MyInventoryComponent.h"
#include "MyPlayerController.h"
#include "MyStatComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Net/UnrealNetwork.h"

std::mutex AMyPlayerState::TeamAssignMutex;

AMyPlayerState::AMyPlayerState()
	: State(EMyCharacterState::Unknown),
	  Team(EMyTeam::Unknown),
	  Kill(0),
	  Death(0),
	  Assist(0),
	  Health(0),
	  Money(0),
      Weapon(nullptr),
      CurrentItem(nullptr)
{
	StatComponent = CreateDefaultSubobject<UMyStatComponent>(TEXT("StatComponent"));
	InventoryComponent = CreateDefaultSubobject<UMyInventoryComponent>(TEXT("InventoryComponent"));
}

void AMyPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (HasLocalNetOwner())
	{
		const auto& PlayerController = Cast<AMyPlayerController>(GetOwner());

		if (!IsValid(PlayerController))
		{
			LOG_FUNC(LogTemp, Error, "PlayerController is not valid");
			return;
		}

		const auto& HUD = Cast<AMyInGameHUD>(PlayerController->GetHUD());

		if (IsValid(HUD))
		{
			LOG_FUNC(LogTemp, Warning, "Bind Player");
			HUD->BindPlayer(this);
		}
		else
		{
			LOG_FUNC(LogTemp, Error, "HUD is not valid");
		}

		const auto& GameState = GetWorld()->GetGameState<AMyGameState>();

		if (IsValid(GameState))
		{
			GameState->BindOnRoundProgressChanged(this, &AMyPlayerState::HandleRoundProgress);
		}
		else
		{
			LOG_FUNC(LogTemp, Error, "GameState is not valid");
		}
	}
}

float AMyPlayerState::TakeDamage(
	float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser
)
{
	SetHP(Health - DamageAmount);
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

int32 AMyPlayerState::GetDamage() const
{
	return StatComponent->GetDamage();
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyPlayerState, Team);
	DOREPLIFETIME(AMyPlayerState, Kill);
	DOREPLIFETIME(AMyPlayerState, Death);
	DOREPLIFETIME(AMyPlayerState, Assist);
	DOREPLIFETIME(AMyPlayerState, State);
	DOREPLIFETIME(AMyPlayerState, Health);
	DOREPLIFETIME(AMyPlayerState, Money);
	DOREPLIFETIME(AMyPlayerState, InventoryComponent);
	DOREPLIFETIME(AMyPlayerState, StatComponent);
	DOREPLIFETIME(AMyPlayerState, Weapon);
	DOREPLIFETIME(AMyPlayerState, CurrentItem);
}

void AMyPlayerState::HandleRoundProgress(EMyRoundProgress RoundProgress) const
{
	const auto& MyController = Cast<AMyPlayerController>(GetOwner());
	const auto& MyCharacter = Cast<AMyCharacter>(MyController->GetPawn());

	if (!HasAuthority())
	{
		Server_HandleRoundProgress(MyCharacter);
	}
	else
	{
		const auto& GameState = GetWorld()->GetGameState<AMyGameState>();

		if (!IsValid(GameState))
		{
			LOG_FUNC(LogTemp, Error, "GameState is not valid");
			return;
		}

		HandleRoundProgressChanged(MyCharacter, GameState->GetState());
	}
}

void AMyPlayerState::Server_HandleRoundProgress_Implementation(AMyCharacter* Character) const
{
	const auto& GameState = GetWorld()->GetGameState<AMyGameState>();

	HandleRoundProgressChanged(Character, GameState->GetState());
}

void AMyPlayerState::OnRep_StateChanged() const
{
	OnStateChanged.Broadcast(Team, State);
}

void AMyPlayerState::OnRep_HealthChanged() const
{
	OnHPChanged.Broadcast(GetPlayerId(), GetHPRatio());
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

void AMyPlayerState::Use(const int32 Index)
{
	CurrentItem = InventoryComponent->Use(Index);
}

void AMyPlayerState::SetState(const EMyCharacterState NewState)
{
	State = NewState;

	if (HasAuthority())
	{
		OnStateChanged.Broadcast(Team, State);
	}
}

float AMyPlayerState::GetHPRatio() const
{
	return FMath::Clamp((float)Health / (float)StatComponent->GetMaxHealth(), 0.f, 1.f);
}

void AMyPlayerState::SetHP(const int32 NewHP)
{
	LOG_FUNC_PRINTF(LogTemp, Warning, "SetHP: %d", NewHP);
	Health = FMath::Clamp(NewHP, 0, StatComponent->GetMaxHealth());

	if (HasAuthority())
	{
		OnHPChanged.Broadcast(GetPlayerId(), GetHPRatio());
	}
	
}

void AMyPlayerState::AddMoney(const int32 Amount)
{
	Money += Amount;

	// Server does not participate in the replication.
	if (HasAuthority())
	{
		OnMoneyChanged.Broadcast(Money);
	}
}

void AMyPlayerState::SetWeapon(AMyWeapon* NewWeapon)
{
	Weapon = NewWeapon;
}
	