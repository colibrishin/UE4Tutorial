// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWeaponStatComponent.h"

#include "MyGameInstance.h"

// Sets default values for this component's properties
UMyWeaponStatComponent::UMyWeaponStatComponent()
	: ID(0),
	  Damage(0)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	bWantsInitializeComponent = true;
}


// Called when the game starts
void UMyWeaponStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UMyWeaponStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	const UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());

	if (IsValid(GameInstance))
	{
		const auto WeaponStatData = GameInstance->GetWeaponValue(ID);
		Damage = WeaponStatData->Damage;
	}
}

