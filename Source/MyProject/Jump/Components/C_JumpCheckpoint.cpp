// Fill out your copyright notice in the Description page of Project Settings.


#include "C_JumpCheckpoint.h"

#include "GameFramework/PawnMovementComponent.h"

#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Jump/Actors/A_JumpFloor.h"
#include "MyProject/Jump/Frameworks/GS_Jump.h"
#include "MyProject/Private/Utilities.hpp"

void UC_JumpCheckpoint::MarkCheckpoint(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult
)
{
	if (const AA_Character* Character = Cast<AA_Character>(OtherActor))
	{
		if (Character->GetMovementComponent()->IsFalling())
		{
			return;
		}
		
		if (!bMarkedPreviously)
		{
			LOG_FUNC(LogTemp, Log, "Marking checkpoint");
			const auto GameState = GetWorld()->GetGameState<AGS_Jump>();
			GameState->SetLastCheckPoint(this);
			bMarkedPreviously = true;
		}
	}
}

// Sets default values for this component's properties
UC_JumpCheckpoint::UC_JumpCheckpoint()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UC_JumpCheckpoint::BeginPlay()
{
	Super::BeginPlay();

	if (UMeshComponent* MeshComponent = Cast<AA_JumpFloor>(GetOwner())->GetCollisionVolumeMesh())
	{
		MeshComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &UC_JumpCheckpoint::MarkCheckpoint);
	}
}

// Called every frame
void UC_JumpCheckpoint::TickComponent(float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime , TickType , ThisTickFunction);

	// ...
}

