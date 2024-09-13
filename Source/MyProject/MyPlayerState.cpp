// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyPlayerState.h"

#include "MyAIController.h"

#include "GameFramework/GameStateBase.h"
#include "MyCharacter.h"
#include "MyGameState.h"
#include "MyProject/Components/MyInventoryComponent.h"
#include "MyPlayerController.h"
#include "MyProject/Components/MyStatComponent.h"
#include "MyWeapon.h"
#include "Components/C_PickUp.h"
#include "Components/C_Buy.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogPlayerState);

std::mutex AMyPlayerState::TeamAssignMutex;

AMyPlayerState::AMyPlayerState()
	: State(EMyCharacterState::Unknown),
	  Team(EMyTeam::Unknown),
	  Kill(0),
	  Death(0),
	  Assist(0),
	  Health(0),
	  Money(0),
      PickUpOfHandObject(nullptr)
{
	StatComponent = CreateDefaultSubobject<UMyStatComponent>(TEXT("StatComponent"));
	InventoryComponent = CreateDefaultSubobject<UMyInventoryComponent>(TEXT("InventoryComponent"));
	BuyComponent = CreateDefaultSubobject<UC_Buy>(TEXT("BuyComponent"));
}

void AMyPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

float AMyPlayerState::TakeDamage(
	float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser
)
{
	SetHP(Health - DamageAmount);

	if (HasAuthority())
	{
		// todo: AIController + Player State?
		if (Cast<AAIController>(EventInstigator))
		{
			return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		}

		const auto& DamageGiver = Cast<AMyPlayerController>(EventInstigator)->GetPlayerState<AMyPlayerState>();
		const auto& Victim = Cast<AMyPlayerController>(GetOwner())->GetPlayerState<AMyPlayerState>();
		const auto& KillerWeapon = Cast<AMyWeapon>(DamageCauser);

		if (GetHP() <= 0)
		{
			OnKillOccurred.Broadcast(DamageGiver, Victim, KillerWeapon);
		}
		else
		{
			if (HasAuthority())
			{
				OnDamageTaken.Broadcast(DamageGiver);
				Client_OnDamageTaken(DamageGiver);
			}
		}
	}

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

int32 AMyPlayerState::GetDamage() const
{
	return StatComponent->GetDamage();
}

void AMyPlayerState::Reset()
{
	Super::Reset();

	if (HasAuthority())
	{
		LOG_FUNC(LogPlayerState, Log, "Reset PlayerState");

		if (State != EMyCharacterState::Alive)
		{
			PickUpOfHandObject->OnObjectDrop.Broadcast(Cast<IPickableObject>(GetPawn()));
			PickUpOfHandObject = nullptr;
			InventoryComponent->Clear();
		}

		SetState(EMyCharacterState::Alive);
		SetHP(StatComponent->GetMaxHealth());
		// todo: Add money by winning or losing
	}
}

void AMyPlayerState::IncrementKills()
{
	if (HasAuthority())
	{
		Kill++;
	}
}

void AMyPlayerState::IncrementDeaths()
{
	if (HasAuthority())
	{
		Death++;
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
	DOREPLIFETIME(AMyPlayerState, Health);
	DOREPLIFETIME(AMyPlayerState, Money);
	DOREPLIFETIME(AMyPlayerState, InventoryComponent);
	DOREPLIFETIME(AMyPlayerState, StatComponent);
	DOREPLIFETIME(AMyPlayerState, BuyComponent);
	DOREPLIFETIME(AMyPlayerState, PickUpOfHandObject);
}

void AMyPlayerState::OnRep_HealthChanged() const
{
	OnHPChanged.Broadcast(GetHPRatio());
}

void AMyPlayerState::OnRep_HandChanged(UC_PickUp* PreviousHand)
{
	OnHandChanged.Broadcast(PreviousHand, PickUpOfHandObject, this);
}

void AMyPlayerState::Client_OnDamageTaken_Implementation(AMyPlayerState* DamageGiver)
{
	OnDamageTaken.Broadcast(DamageGiver);
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

void AMyPlayerState::SetHand(UC_PickUp* InPickUp)
{
	OnHandChanged.Broadcast(PickUpOfHandObject, InPickUp, this);
	PickUpOfHandObject = InPickUp;
}

void AMyPlayerState::SetState(const EMyCharacterState NewState)
{
	State = NewState;

	if (HasAuthority())
	{
		OnStateChanged.Broadcast(this, State);
	}
}

float AMyPlayerState::GetHPRatio() const
{
	return FMath::Clamp((float)Health / (float)StatComponent->GetMaxHealth(), 0.f, 1.f);
}

UC_Buy* AMyPlayerState::GetBuyComponent() const
{
	return BuyComponent;
}

void AMyPlayerState::SetHP(const int32 NewHP)
{
	LOG_FUNC_PRINTF(LogTemp, Warning, "SetHP: %d", NewHP);
	Health = FMath::Clamp(NewHP, 0, StatComponent->GetMaxHealth());

	if (HasAuthority())
	{
		if (Health <= 0 && State != EMyCharacterState::Dead)
		{
			SetState(EMyCharacterState::Dead);
		}

		OnHPChanged.Broadcast(GetHPRatio());
	}
	
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
