// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PickUp.h"

#include "..\Interfaces\PickingUp.h"

#include "Camera/CameraComponent.h"

#include "GameFramework/MovementComponent.h"

#include "MyProject/Actors/BaseClass/A_Collectable.h"
#include "MyProject/Private/Utilities.hpp"

DEFINE_LOG_CATEGORY(LogPickUp);

// Sets default values for this component's properties
UC_PickUp::UC_PickUp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}


void UC_PickUp::SetActive(bool bNewActive, bool bReset)
{
	const bool bPreviousActive = IsActive();

	Super::SetActive(bNewActive , bReset);

	if (bNewActive != bPreviousActive && GetNetMode() != NM_Client)
	{
		if (!bNewActive)
		{
			OnObjectPickUp.RemoveAll(this);
			OnObjectDrop.RemoveAll(this);

			if (UMeshComponent* MeshComponent = GetOwner()->GetComponentByClass<UMeshComponent>())
			{
				MeshComponent->OnComponentBeginOverlap.RemoveAll(this);
			}
		}
		else
		{
			AttachEventHandlers();
		}
	}
}

void UC_PickUp::AttachEventHandlers()
{
	OnObjectPickUp.AddUniqueDynamic(this , &UC_PickUp::OnPickUpCallback);
	OnObjectDrop.AddUniqueDynamic(this , &UC_PickUp::OnDropCallback);

	if (UMeshComponent* MeshComponent = GetOwner()->GetComponentByClass<UMeshComponent>())
	{
		MeshComponent->OnComponentBeginOverlap.AddUniqueDynamic(this , &UC_PickUp::OnBeginOverlap);
	}
}

// Called when the game starts
void UC_PickUp::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() != NM_Client)
	{
		if (UMeshComponent* MeshComponent = GetOwner()->GetComponentByClass<UMeshComponent>())
		{
			MeshComponent->SetGenerateOverlapEvents(true);
		}

		AttachEventHandlers();
	}
}

void UC_PickUp::OnBeginOverlap(
	UPrimitiveComponent* /*OverlappedComponent*/ , AActor* OtherActor ,
	UPrimitiveComponent* /*OtherComp*/ , int32 /*OtherBodyIndex*/ , bool /*bFromSweep*/ ,
	const FHitResult& /*SweepResult*/
)
{
	if (Cast<IPickingUp>(OtherActor))
	{
		OnObjectPickUp.Broadcast(TScriptInterface<IPickingUp>(OtherActor), true);
	}
}

void UC_PickUp::OnPickUpCallback(TScriptInterface<IPickingUp> InCaller, const bool bCallPickUp)
{
	if (!InCaller)
	{
		LOG_FUNC(LogPickUp , Error , "Caught invalid pickup object!");
		return;
	}

	LOG_FUNC_PRINTF(LogPickUp , Log , "Caught pickup : %s" , *InCaller->_getUObject()->GetName());

	OnObjectPickUp.RemoveAll(this);
	OnObjectDrop.AddUniqueDynamic(this , &UC_PickUp::OnDropCallback);

	if (bCallPickUp)
	{
		if (GetNetMode() != NM_Client)
		{
			InCaller->PickUp(this);

			// Assuming the object is cloned into child actor component or consumed etc;
			GetOwner()->Destroy(true);
		}
	}
}

void UC_PickUp::OnDropCallback(TScriptInterface<IPickingUp> InCaller, const bool bCallDrop)
{
	if (!InCaller)
	{
		LOG_FUNC(LogPickUp , Error , "Caught invalid drop object!");
		return;
	}

	LOG_FUNC_PRINTF(LogPickUp , Log , "Caught drop : %s" , *InCaller->_getUObject()->GetName());

	OnObjectDrop.RemoveAll(this);
	OnObjectPickUp.AddUniqueDynamic(this , &UC_PickUp::OnPickUpCallback);

	if (bCallDrop)
	{
		if (GetNetMode() != NM_Client)
		{
			const AActor* PickingObject = Cast<AActor>(InCaller.GetInterface());
			FVector Origin, Extents;
			PickingObject->GetActorBounds(true, Origin, Extents);

			FVector DropLocation = PickingObject->GetActorLocation();
			FVector ForwardVector = PickingObject->GetActorForwardVector();

			if (const UCameraComponent* CameraComponent = PickingObject->GetComponentByClass<UCameraComponent>())
			{
				ForwardVector = CameraComponent->GetForwardVector();
			}

			const FRotator ForwardRotator = ForwardVector.Rotation();
			const FVector RotatedExtent = ForwardRotator.RotateVector(Extents);
			const FVector RePickPrevention = {RotatedExtent.X, RotatedExtent.Y, Extents.Z};

			const FTransform Transform
			{
				FQuat::Identity,
				DropLocation + RePickPrevention,
				FVector::OneVector
			};
			
			// Clone the object before destroyed => ChildActorComponent->DestroyChildActor();
			AA_Collectable* InObject = Cast<AA_Collectable>(GetOwner());
			AA_Collectable* Cloned = FCollectableUtility::CloneChildActor
			(
				InObject,
				Transform,
				[this, &ForwardVector](AActor* InActor)
				{
					AA_Collectable* InCollectable = Cast<AA_Collectable>(InActor); 
					
					InCollectable->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
					InCollectable->SetPhysics(true);
					InCollectable->SetReplicates(true);
					InCollectable->SetReplicateMovement(true);

					// Reset velocity;
					if (UPrimitiveComponent* CollisionComponent = InCollectable->GetCollisionComponent())
					{
						CollisionComponent->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
						CollisionComponent->SetAllPhysicsAngularVelocityInRadians(FVector::ZeroVector);
						CollisionComponent->AddForce(ForwardVector * 50.f);
					}

					OnObjectDropPreSpawned.Broadcast(InActor);
				}
			);

			// Object destruction should be handled in InCaller's Drop;
			InCaller->Drop(this);
		}
	}
}
