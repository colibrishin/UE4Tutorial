// Fill out your copyright notice in the Description page of Project Settings.


#include "A_JumpFloor.h"

#include "MyProject/Jump/Components/C_JumpCheckpoint.h"
#include "MyProject/Jump/Components/Assets//C_JumpFloorAsset.h"

// Sets default values
AA_JumpFloor::AA_JumpFloor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	CollisionVolumeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionVolumeMesh"));
	CheckpointComponent = CreateDefaultSubobject<UC_JumpCheckpoint>(TEXT("CheckpointComponent"));
	AssetComponent = CreateDefaultSubobject<UC_JumpFloorAsset>(TEXT("AssetComponent"));
	
	SetRootComponent(Mesh);
	Mesh->SetGenerateOverlapEvents(false);

	CollisionVolumeMesh->SetupAttachment(Mesh);
	CollisionVolumeMesh->SetVisibleFlag(false);
	CollisionVolumeMesh->SetGenerateOverlapEvents(true);
	CollisionVolumeMesh->SetCollisionProfileName(TEXT("OverlapAll"));
	// Slightly large mesh than actual mesh for overlap detection;
	CollisionVolumeMesh->SetWorldScale3D(FVector::OneVector * (1.f + FLT_EPSILON));
}

#if WITH_EDITOR
void AA_JumpFloor::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	if (const FName PropertyName = PropertyChangedEvent.Property->GetFName();
		PropertyName == GET_MEMBER_NAME_CHECKED(AA_JumpFloor, Mesh))
	{
		CollisionVolumeMesh->SetStaticMesh(Mesh->GetStaticMesh());
	}
}
#endif

// Called when the game starts or when spawned
void AA_JumpFloor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AA_JumpFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

