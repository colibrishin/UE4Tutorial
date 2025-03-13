// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PickUp.h"

#include "..\Interfaces\PickingUp.h"

#include "Camera/CameraComponent.h"
#include "Components/ShapeComponent.h"

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

void UC_PickUp::AttachEventHandlers(const bool bEnable, const EPickUp PickUpOrDrop)
{
	if ( GetNetMode() == NM_Client )
	{
		return;
	}

	if ( bEnable )
	{
		switch ( PickUpOrDrop )
		{
		case EPickUp::PickUp:
		{
			OnObjectPickUp.AddUniqueDynamic( this , &UC_PickUp::OnPickUpCallback );
			if ( OnObjectDrop.Contains( this , "OnDropCallback" ) )
			{
				OnObjectDrop.Remove( this , "OnDropCallback" );
			}

			if ( UPrimitiveComponent* CollisionComponent = GetOwner()->GetComponentByClass<UPrimitiveComponent>() )
			{
				CollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic( this , &UC_PickUp::OnBeginOverlap );
			}
			break;
		}
		case EPickUp::Drop:
		{
			if ( OnObjectPickUp.Contains( this , "OnPickUpCallBack" ) )
			{
				OnObjectPickUp.Remove( this , "OnPickUpCallback" );
			}
			OnObjectDrop.AddUniqueDynamic( this , &UC_PickUp::OnDropCallback );

			if ( UPrimitiveComponent* CollisionComponent = GetOwner()->GetComponentByClass<UPrimitiveComponent>();
				 CollisionComponent && CollisionComponent->OnComponentBeginOverlap.Contains(this, "OnBeginOverlap") )
			{
				CollisionComponent->OnComponentBeginOverlap.Remove( this , "OnBeginOverlap" );
			}
			break;
		}
		default:
			check( false );
			break;
		}
	}
	else 
	{
		OnObjectPickUp.RemoveAll( this );
		OnObjectDrop.RemoveAll( this );

		if ( UPrimitiveComponent* CollisionComponent = GetOwner()->GetComponentByClass<UPrimitiveComponent>();
			 CollisionComponent && CollisionComponent->OnComponentBeginOverlap.Contains(this, "OnBeginOverlap") )
		{
			CollisionComponent->OnComponentBeginOverlap.Remove( this, "OnBeginOverlap" );
		}
	}
}

// Called when the game starts
void UC_PickUp::BeginPlay()
{
	Super::BeginPlay();
}

void UC_PickUp::OnBeginOverlap(
	UPrimitiveComponent* /*OverlappedComponent*/ , AActor* OtherActor ,
	UPrimitiveComponent* /*OtherComp*/ , int32 /*OtherBodyIndex*/ , bool /*bFromSweep*/ ,
	const FHitResult& /*SweepResult*/
)
{
	if ( Cast<IPickingUp>(OtherActor) )
	{
		OnObjectPickUp.Broadcast(TScriptInterface<IPickingUp>(OtherActor), true);
	}
}

void UC_PickUp::OnPickUpCallback(TScriptInterface<IPickingUp> InCaller, const bool bCallPickUp)
{
	if ( GetNetMode() == NM_Client )
	{
		return;
	}

	if (!InCaller)
	{
		LOG_FUNC(LogPickUp , Error , "Caught invalid pickup object!");
		return;
	}

	AttachEventHandlers( true , EPickUp::Drop );

	if (bCallPickUp)
	{
		LOG_FUNC_PRINTF( LogPickUp , Log , "Caught pickup : %s" , *InCaller->_getUObject()->GetName() );
		InCaller->PickUp( this );

		// Assuming the object is cloned into child actor component or consumed etc;
		GetOwner()->Destroy( true );
	}
}

void UC_PickUp::OnDropCallback(TScriptInterface<IPickingUp> InCaller, const bool bCallDrop)
{
	if ( GetNetMode() == NM_Client )
	{
		return;
	}

	if (!InCaller)
	{
		LOG_FUNC(LogPickUp , Error , "Caught invalid drop object!");
		return;
	}

	AttachEventHandlers( false , EPickUp::Drop );

	if ( bCallDrop )
	{
		LOG_FUNC_PRINTF( LogPickUp , Log , "Caught drop : %s" , *InCaller->_getUObject()->GetName() );

		const AActor* PickingObject = Cast<AActor>( InCaller.GetInterface() );
		FVector Origin , Extents;
		PickingObject->GetActorBounds( true , Origin , Extents );

		FVector DropLocation = PickingObject->GetActorLocation();
		FVector ForwardVector = PickingObject->GetActorForwardVector();

		if ( const UCameraComponent* CameraComponent = PickingObject->GetComponentByClass<UCameraComponent>() )
		{
			ForwardVector = CameraComponent->GetForwardVector();
		}

		const FRotator ForwardRotator = ForwardVector.Rotation();
		const FVector RotatedExtent = ForwardRotator.RotateVector( Extents );
		const FVector RePickPrevention = { RotatedExtent.X, RotatedExtent.Y, Extents.Z };

		const FTransform Transform
		{
			FQuat::Identity,
			DropLocation + RePickPrevention,
			FVector::OneVector
		};

		// Clone the object before destroyed => ChildActorComponent->DestroyChildActor();
		AA_Collectable* InObject = Cast<AA_Collectable>( GetOwner() );
		AA_Collectable* Cloned = FCollectableUtility::CloneChildActor
		(
			InObject ,
			Transform ,
			[this , &ForwardVector]( AActor* InActor )
			{
				AA_Collectable* InCollectable = Cast<AA_Collectable>( InActor );

				InCollectable->DetachFromActor( FDetachmentTransformRules::KeepRelativeTransform );
				
				// Disable the client side physics simulation
				InCollectable->SetPhysicsInClient( false );
				InCollectable->SetCollisionTypeInClient( ECollisionEnabled::NoCollision );

				// Enable the server side physics simulation
				InCollectable->GetCollisionComponent()->SetSimulatePhysics( true );
				InCollectable->GetCollisionComponent()->SetCollisionEnabled( ECollisionEnabled::QueryAndPhysics );

				InCollectable->SetReplicates( true );
				InCollectable->SetReplicateMovement( true );
				InCollectable->bAlwaysRelevant = true;

				// Reset any velocities and throw the object
				if ( UShapeComponent* CollisionComponent = InCollectable->GetCollisionComponent() )
				{
					CollisionComponent->SetAllPhysicsLinearVelocity( FVector::ZeroVector );
					CollisionComponent->SetAllPhysicsAngularVelocityInRadians( FVector::ZeroVector );
					CollisionComponent->AddForce( ForwardVector * 50.f );
				}

				OnObjectDropPreSpawned.Broadcast( InActor );
			}
		);
		Cloned->GetComponentByClass<UC_PickUp>()->AttachEventHandlers( true , EPickUp::PickUp );

		// Object destruction should be handled in InCaller's Drop;
		InCaller->Drop( this );
	}
}
