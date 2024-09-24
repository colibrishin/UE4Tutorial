// Fill out your copyright notice in the Description page of Project Settings.


#include "A_JumpMovingFloor.h"

#include "MyProject/Jump/Components/C_JumpFloorMovement.h"
#include "MyProject/Jump/Components/Assets/C_JumpMovingFloorAsset.h"


// Sets default values
AA_JumpMovingFloor::AA_JumpMovingFloor(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UC_JumpMovingFloorAsset>(TEXT("AssetComponent")))
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloorMovementComponent = CreateDefaultSubobject<UC_JumpFloorMovement>(TEXT("FloorMovementComponent"));
}

// Called when the game starts or when spawned
void AA_JumpMovingFloor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AA_JumpMovingFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

