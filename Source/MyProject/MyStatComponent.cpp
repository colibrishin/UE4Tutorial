// Fill out your copyright notice in the Description page of Project Settings.


#include "MyStatComponent.h"

#include "MyGameInstance.h"

#include "Engine/World.h"

// Sets default values for this component's properties
UMyStatComponent::UMyStatComponent()
	: Level(1),
	  Damage(0),
	  Health(0)
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
		const auto& Data = GameInstance->GetValue(Level);

		// IsValid 스킵

		Level     = Data->Level;
		Damage    = Data->Damage;
		Health = Data->MaxHealth;
	}

}
