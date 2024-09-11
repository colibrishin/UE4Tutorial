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
	: ID(0),
	  SlotType(EMySlotType::Unknown)
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
		const UMyCollectableDataAsset* Asset = Data->CollectableDataAsset;
		check(Asset);

		SlotType                             = Data->CollectableDataAsset->GetSlotType();
		
		AMyCollectable* Collectable = Cast<AMyCollectable>(GetOwner());
		
		if (Asset->IsSkeletal())
		{
			Collectable->SetSkeletalMesh();
			Collectable->GetSkeletalMeshComponent()->SetSkeletalMesh(Asset->GetSkeletalMesh());
		}
		else
		{
			Collectable->SetStaticMesh();
			Collectable->GetStaticMeshComponent()->SetStaticMesh(Asset->GetStaticMesh());
		}
	}
}

void UMyCollectableComponent::OnRep_ID()
{
	UpdateAsset();
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

