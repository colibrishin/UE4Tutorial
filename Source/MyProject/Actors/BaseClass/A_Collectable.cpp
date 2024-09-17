// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Collectable.h"

#include "MyProject/Components/Asset/C_CollectableAsset.h"

#include "Net/UnrealNetwork.h"


// Sets default values
AA_Collectable::AA_Collectable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AssetComponent = CreateDefaultSubobject<UC_CollectableAsset>(TEXT("CollectableAsset"));
	AssetComponent->SetNetAddressable();
	AssetComponent->SetIsReplicated(true);
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
}

// Called every frame
void AA_Collectable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

