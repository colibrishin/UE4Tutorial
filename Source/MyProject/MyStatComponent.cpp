// Fill out your copyright notice in the Description page of Project Settings.


#include "MyStatComponent.h"

#include "Data.h"
#include "MyGameInstance.h"
#include "MyInGameHUD.h"
#include "MyPlayerState.h"

#include "Engine/World.h"

#include "GameFramework/HUD.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UMyStatComponent::UMyStatComponent()
	: Level(1),
	  Damage(0),
	  MaxHealth(0)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bWantsInitializeComponent = true;
}

// Called when the game starts
void UMyStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UMyStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	const UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());

	if (IsValid(GameInstance))
	{
		const FMyStat* Data = GetStatData(this, Level);

		// IsValid 스킵

		Level     = Data->Level;
		Damage    = Data->Damage;
		MaxHealth = Data->MaxHealth;
	}

}
