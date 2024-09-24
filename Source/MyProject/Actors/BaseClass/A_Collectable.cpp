// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Collectable.h"

#include "MyProject/Components/C_PickUp.h"
#include "MyProject/Components/Asset/C_CollectableAsset.h"

#include "Net/UnrealNetwork.h"

const FName AA_Collectable::AssetComponentName (TEXT("AssetComponent"));

// Sets default values
AA_Collectable::AA_Collectable(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AssetComponent = CreateDefaultSubobject<UC_CollectableAsset>(TEXT("AssetComponent"));
	PickUpComponent = CreateDefaultSubobject<UC_PickUp>(TEXT("PickUpComponent"));

	SetRootComponent(PickUpComponent);

	AssetComponent->SetNetAddressable();
	AssetComponent->SetIsReplicated(true);

	bDummy = false;
}

// Called when the game starts or when spawned
void AA_Collectable::BeginPlay()
{
	Super::BeginPlay();
	
}

void AA_Collectable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AA_Collectable, AssetComponent);
	DOREPLIFETIME_CONDITION(AA_Collectable, bDummy, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AA_Collectable, Sibling, COND_OwnerOnly);
}

void AA_Collectable::OnRep_Dummy() const
{
	OnDummyFlagSet.Broadcast();
}

// Called every frame
void AA_Collectable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

