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
			FTimerDelegate Delegate;
			Delegate.BindUObject(this, &UC_JumpCheckpoint::CheckOverlapAtTimerEnd, OtherActor);

			// Postpone overlap process for check whether player is still on the floor;
			GetWorld()->GetTimerManager().SetTimer(
				CheckpointTimerHandle,
				Delegate,
				1.f,
				false);
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

void UC_JumpCheckpoint::UnbindDelegate()
{
	if (UMeshComponent* MeshComponent = Cast<AA_JumpFloor>(GetOwner())->GetCollisionVolumeMesh())
	{
		MeshComponent->OnComponentBeginOverlap.RemoveAll(this);
	}
}

void UC_JumpCheckpoint::BindDelegate()
{
	if (UMeshComponent* MeshComponent = Cast<AA_JumpFloor>(GetOwner())->GetCollisionVolumeMesh())
	{
		MeshComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &UC_JumpCheckpoint::MarkCheckpoint);
	}
}

void UC_JumpCheckpoint::CheckOverlapAtTimerEnd(AActor* InOtherActor)
{
	if (bMarkedPreviously)
	{
		return;
	}
	
	if (InOtherActor->IsOverlappingActor(GetOwner()))
	{
		LOG_FUNC_PRINTF(LogTemp, Log, "Marking checkpoint %s", *GetOwner()->GetName());
		const auto GameState = GetWorld()->GetGameState<AGS_Jump>();
		GameState->SetLastCheckPoint(this);
		bMarkedPreviously = true;
		UnbindDelegate();
	}
	else
	{
		LOG_FUNC_PRINTF(LogTemp, Log, "%s overlap prematurely finished, not marking", *GetOwner()->GetName())
	}
}

// Called when the game starts
void UC_JumpCheckpoint::BeginPlay()
{
	Super::BeginPlay();
	BindDelegate();
}

// Called every frame
void UC_JumpCheckpoint::TickComponent(float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime , TickType , ThisTickFunction);

	// ...
}

