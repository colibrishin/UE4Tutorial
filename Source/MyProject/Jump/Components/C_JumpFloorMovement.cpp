// Fill out your copyright notice in the Description page of Project Settings.


#include "C_JumpFloorMovement.h"


// Sets default values for this component's properties
UC_JumpFloorMovement::UC_JumpFloorMovement()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UC_JumpFloorMovement::Flipflop()
{
	bFlip = !bFlip;
}


// Called when the game starts
void UC_JumpFloorMovement::BeginPlay()
{
	Super::BeginPlay();

	// ...
	GetWorld()->GetTimerManager().SetTimer(
		MoveTimerHandle,
		this,
		&UC_JumpFloorMovement::Flipflop,
		Duration,
		true);
}


// Called every frame
void UC_JumpFloorMovement::TickComponent(
	float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime , TickType , ThisTickFunction);

	// ...
	if (bMoving)
	{
		const FVector MoveDirection = bFlip ? -Direction : Direction;
		const float Speed = Length / Duration;
		const float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MoveTimerHandle);
		float CurrentAcceleration = 1.f;
	
		if (Acceleration)
		{
			CurrentAcceleration = Acceleration->GetFloatValue(ElapsedTime / Duration);
		}

		const FVector FinalMovement = MoveDirection * (Speed * CurrentAcceleration * DeltaTime);
		GetOwner()->AddActorWorldOffset(FinalMovement);
	}

	if (bRotating)
	{
		const FRotator SpinRotation = bFlip ? Rotation.GetInverse() : Rotation;
		const float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MoveTimerHandle);
		const float Alpha = ElapsedTime / Duration;
		float CurrentAcceleration = 1.f;

		if (Acceleration)
		{
			CurrentAcceleration = Acceleration->GetFloatValue(Alpha);
		}
		
		const FRotator FinalRotation = SpinRotation * Alpha * CurrentAcceleration * DeltaTime;
		GetOwner()->AddActorWorldRotation(FinalRotation);		
	}
}

