// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetFetchable.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "MyProject/Private/Enum.h"

#include "MyProject/Components/Asset/C_CollectableAsset.h"

void IAssetFetchable::UpdateCollisionComponent( USceneComponent* RootComponent , USceneComponent* ParentComponent , UShapeComponent* NewComponent , const FName& CollisionProfileName , const EMultiShapeType InCollisionType , const FBoxSphereBounds& Bounds , const FVector& CustomScale )
{
	if (AActor* Actor = Cast<AActor>(this)) 
	{
		const float ScaleDistanced = FVector::Distance( FVector::ZeroVector , CustomScale );

		switch ( InCollisionType )
		{
		case EMultiShapeType::Box:
		{
			Cast<UBoxComponent>( NewComponent )->SetBoxExtent( Bounds.BoxExtent * CustomScale );
			break;
		}
		case EMultiShapeType::Sphere:
		{
			Cast<USphereComponent>( NewComponent )->SetSphereRadius( Bounds.SphereRadius * ScaleDistanced );
			break;
		}
		case EMultiShapeType::Capsule:
		{
			// todo: accurate estimation
			Cast<UCapsuleComponent>( NewComponent )->SetCapsuleRadius( Bounds.SphereRadius * ScaleDistanced );
			Cast<UCapsuleComponent>( NewComponent )->SetCapsuleHalfHeight( Bounds.BoxExtent.Z * CustomScale.Z );
			break;
		}
		default: check( false );
		}

		Actor->SetRootComponent(NewComponent);

		if (ParentComponent)
		{
			NewComponent->AttachToComponent
			(
				ParentComponent,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);
		}

		if (RootComponent) 
		{
			RootComponent->AttachToComponent(NewComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}

		// lazy initialization of collision component;
		NewComponent->SetCollisionProfileName(CollisionProfileName);
		NewComponent->SetSimulatePhysics(false);
		NewComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndProbe);
		NewComponent->SetAllUseCCD(true);
	}	
}

// Add default functionality here for any IAssetFetchable functions that are not pure virtual.
void IAssetFetchable::PostFetchAsset() {}
