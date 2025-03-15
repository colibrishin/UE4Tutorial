// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Health.h"

#include "MyProject/Private/Utilities.hpp"

#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogHealthComponent);

// Sets default values for this component's properties
UC_Health::UC_Health()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UC_Health::Decrease(const int32 InValue)
{
	if (GetNetMode() == NM_Client)
	{
		return;
	}
	
	// Eliminating overflow case.
	if (InValue < 0)
	{
		LOG_FUNC(LogHealthComponent, Error, "Value should be given in positive value!");
		return;
	}

	const auto& NewValue = Health - InValue;

	if (Health < std::numeric_limits<int32>::min() + InValue)
	{
		LOG_FUNC(LogHealthComponent, Warning, "Underflow! Assuming the value to 0");
		Health = 0;
	}
	else if (NewValue <= 0)
	{
		Health = 0;
	}
	else
	{
		Health = NewValue;
	}

	LOG_FUNC_PRINTF(LogHealthComponent, Log, "Health changed to %d", Health);
	const auto PreviousHealth = Health;

	OnHPChanged.Broadcast(PreviousHealth, Health);
	NotifyHPRatioUpdate();
}

void UC_Health::Increase(const int32 InValue)
{
	if (GetNetMode() == NM_Client)
	{
		return;
	}

	// Eliminating underflow case.
	if (InValue < 0)
	{
		LOG_FUNC(LogHealthComponent, Error, "Value should be given in positive value!");
		return;
	}

	const auto& NewValue = Health + InValue;
	
	if (Health > std::numeric_limits<int32>::max() - InValue)
	{
		LOG_FUNC(LogHealthComponent, Warning, "Overflow! Assuming the value to MaxHealth");
		Health = MaxHealth;
	}
	else if (NewValue >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health = NewValue;
	}

	LOG_FUNC_PRINTF(LogHealthComponent, Log, "Health changed to %d", NewValue);

	const auto PreviousHealth = Health;

	OnHPChanged.Broadcast(PreviousHealth, Health);
	NotifyHPRatioUpdate();
}


// Called when the game starts
void UC_Health::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated( true );
	// ...
	
}

void UC_Health::NotifyHPRatioUpdate() const
{
	OnHPChangedRatio.Broadcast( FMath::Clamp( (float)Health / (float)MaxHealth , 0.f , 1.f ) );
}

void UC_Health::OnRep_HP(const int32 InOld) const
{
	OnHPChanged.Broadcast(InOld, Health);
	NotifyHPRatioUpdate();
}

void UC_Health::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UC_Health, Health);
}
