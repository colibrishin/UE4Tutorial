// Fill out your copyright notice in the Description page of Project Settings.


#include "C_JumpMovingFloorAsset.h"

#include "MyProject/Jump/Components/C_JumpFloorMovement.h"
#include "MyProject/Jump/DataAssets/DA_JumpMovingFloor.h"


// Sets default values for this component's properties
UC_JumpMovingFloorAsset::UC_JumpMovingFloorAsset()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UC_JumpMovingFloorAsset::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UC_JumpMovingFloorAsset::TickComponent(
	float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime , TickType , ThisTickFunction);

	// ...
}

void UC_JumpMovingFloorAsset::ApplyAsset()
{
	Super::ApplyAsset();

	const UDA_JumpMovingFloor* Asset = GetAsset<UDA_JumpMovingFloor>();
	
	if (UC_JumpFloorMovement* FloorMovement = GetOwner()->GetComponentByClass<UC_JumpFloorMovement>())
	{
		FloorMovement->Acceleration = Asset->GetAcceleration();
		FloorMovement->Direction = Asset->GetDirection();
		FloorMovement->Length = Asset->GetLength();
		FloorMovement->Duration = Asset->GetDuration();
	}
}

