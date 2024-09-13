// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCollectableComponent.h"

#include "MyProject/MyCollectable.h"
#include "MyProject/MyCollectableDataAsset.h"
#include "MyProject/Private/Utilities.hpp"
#include "MyProject/Private/Enum.h"
#include "MyProject/Private/Data.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UMyCollectableComponent::UMyCollectableComponent()
	: ID(0)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	SetNetAddressable();
	SetIsReplicatedByDefault(true);
}

void UMyCollectableComponent::SetID(const int32 InID)
{
	ID = InID;
	UpdateAsset();
}

EMySlotType UMyCollectableComponent::GetSlotType() const
{
	if (DataAsset)
	{
		return DataAsset->GetSlotType();
	}
		
	return EMySlotType::Unknown;
}

void UMyCollectableComponent::OnRep_ID()
{
	UpdateAsset();
}


void UMyCollectableComponent::ApplyAsset() const
{
	if (!DataAsset)
	{
		LOG_FUNC(LogTemp, Error, "Asset is not initialized");
		return;
	}
	
	if (const AMyCollectable* Collectable = Cast<AMyCollectable>(GetOwner()))
	{
		Collectable->GetSkeletalMeshComponent()->SetSkeletalMesh(DataAsset->GetSkeletalMesh());
	}
}

// Called when the game starts
void UMyCollectableComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UMyCollectableComponent::UpdateAsset()
{
	if (const FMyCollectableData* Data = GetRowData<FMyCollectableData>(GetWorld(), ID))
	{
		LOG_FUNC_PRINTF(LogTemp, Log, "Retrieve the collectable data %d...", ID);
		DataAsset = Data->CollectableDataAsset;
	}
	else
	{
		LOG_FUNC_PRINTF(LogTemp, Error, "Unable to find collectable data %d", ID);
	}
}

void UMyCollectableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMyCollectableComponent, ID);
}

// Called every frame
void UMyCollectableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

