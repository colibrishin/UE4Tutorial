// Fill out your copyright notice in the Description page of Project Settings.


#include "C_JumpFloorMovement.h"

#include "Components/SplineComponent.h"

#include "MyProject/Jump/Actors/A_JumpMovingFloor.h"


// Sets default values for this component's properties
UC_JumpFloorMovement::UC_JumpFloorMovement()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UC_JumpFloorMovement::BeginPlay()
{
	Super::BeginPlay();

	// ...
	GetWorld()->GetTimerManager().SetTimer(
		MoveTimerHandle,
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
	// Use spline component closed loop for flip;
	if (const USplineComponent* SplineComponent = GetOwner()->GetComponentByClass<USplineComponent>())
	{
		const float Elapsed = GetWorld()->GetTimerManager().GetTimerElapsed(MoveTimerHandle);
		
		if (UStaticMeshComponent* StaticMeshComponent = Cast<AA_JumpMovingFloor>(GetOwner())->GetMesh())
		{
			if (bMoving)
			{
				const FVector NextPosition = SplineComponent->GetLocationAtTime(Elapsed, ESplineCoordinateSpace::Local);
				StaticMeshComponent->SetRelativeLocation(NextPosition);	
			}

			if (bRotating)
			{
				const FRotator NextRotation = SplineComponent->GetRotationAtTime(Elapsed, ESplineCoordinateSpace::Local);
				StaticMeshComponent->SetRelativeRotation(NextRotation);
			}
		}
	}
}

