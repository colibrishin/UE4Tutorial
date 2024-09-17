// Fill out your copyright notice in the Description page of Project Settings.


#include "A_C4.h"

#include "MyProject/Components/C_Interactive.h"
#include "MyProject/Private/Enum.h"

#include "Net/UnrealNetwork.h"


// Sets default values
AA_C4::AA_C4()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bUsePredicate = true;
	bNetLoadOnClient = true;
	bReplicates = true;

	InteractiveComponent = CreateDefaultSubobject<UC_Interactive>(TEXT("InteractiveComponent"));
	BombState = EMyBombState::Unknown;
}

float AA_C4::GetElapsedDetonationTime() const
{
	return DetonationTime;
}

float AA_C4::GetDetonationTime() const
{
	return DetonationTime;
}

float AA_C4::GetElapsedPlantTimeRatio() const
{
	if (BombState == EMyBombState::Planting)
	{
		return PlantingTime / InteractiveComponent->GetElapsedTime();
	}

	return 0.f;
}

float AA_C4::GetElapsedDefuseTimeRatio() const
{
	if (BombState == EMyBombState::Defusing)
	{
		return DefusingTime / InteractiveComponent->GetElapsedTime();
	}

	return 0.f;
}

EMyBombState AA_C4::GetBombState() const
{
	return BombState;
}

// Called when the game starts or when spawned
void  AA_C4::BeginPlay()
{
	Super::BeginPlay();
	BombState = EMyBombState::Idle;
	InteractiveComponent->SetDelayTime(PlantingTime);
}

bool AA_C4::PredicateInteraction()
{
	//todo: range check, viewport check

	return true;
}

void AA_C4::Interaction()
{
	if (const EMyBombState PreviousState = BombState;
		BombState == EMyBombState::Planting)
	{
		Planter = InteractiveComponent->GetInteractor();
		BombState = EMyBombState::Planted;
		OnBombStateChanged.Broadcast(PreviousState, BombState, Planter, Defuser);
		InteractiveComponent->SetDelayTime(DefusingTime);

		// todo: drop object and disable physics simulation;
		return;
	}
	else if (BombState == EMyBombState::Defusing)
	{
		Defuser = InteractiveComponent->GetInteractor();
		BombState = EMyBombState::Defused;
		OnBombStateChanged.Broadcast(PreviousState, BombState, Planter, Defuser);
		InteractiveComponent->SetActive(false);
		return;
	}

	ensureAlwaysMsgf(false, TEXT("Uncaught state"));
}

void AA_C4::StartInteraction()
{
	if (const EMyBombState PreviousState = BombState;
		Planter && Defuser && BombState == EMyBombState::Planted)
	{
		BombState = EMyBombState::Defusing;
		Defuser = InteractiveComponent->GetInteractor();
		OnBombStateChanged.Broadcast(PreviousState, BombState, Planter, Defuser);
	}
	else if (Planter && !Defuser && BombState == EMyBombState::Idle)
	{
		BombState = EMyBombState::Planting;
		Planter = InteractiveComponent->GetInteractor();
		OnBombStateChanged.Broadcast(PreviousState, BombState, Planter, Defuser);
	}

	ensureAlwaysMsgf(false, TEXT("Uncaught state"));
}

void AA_C4::StopInteraction()
{
	if (const EMyBombState PreviousState = BombState;
		BombState == EMyBombState::Defusing)
	{
		BombState = EMyBombState::Planted;
		Defuser = nullptr;
		OnBombStateChanged.Broadcast(PreviousState, BombState, Planter, Defuser);
		return;
	}
	else if (BombState == EMyBombState::Planting)
	{
		BombState = EMyBombState::Idle;
		Planter = nullptr;
		OnBombStateChanged.Broadcast(PreviousState, BombState, Planter, Defuser);
		return;
	}

	ensureAlwaysMsgf(false, TEXT("Uncaught state"));
}

void AA_C4::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AA_C4, InteractiveComponent);
	DOREPLIFETIME(AA_C4, BombState);
	DOREPLIFETIME(AA_C4, Planter);
	DOREPLIFETIME(AA_C4, Defuser);
}

void AA_C4::OnRep_BombState(const EMyBombState InOldBombState) const
{
	OnBombStateChanged.Broadcast(InOldBombState, BombState, Planter, Defuser);
}

