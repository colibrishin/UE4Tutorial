// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/MyCollectableComponent.h"
#include "Private/Utilities.hpp"

// Sets default values for this component's properties
UMyCollectableComponent::UMyCollectableComponent()
	: ID(0),
	  SlotType(EMySlotType::Unknown),
	  Stat(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	bWantsInitializeComponent = true;
}


// Called when the game starts
void UMyCollectableComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UMyCollectableComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (ID == 0)
	{
		LOG_FUNC_PRINTF(LogTemp, Error, "ID is not set for %s", *GetOwner()->GetName());
		return;
	}

	Stat = GetRowData<FMyCollectableData>(GetWorld(), ID);

	if (Stat)
	{
		SlotType = Stat->SlotType;
	}
}


// Called every frame
void UMyCollectableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

