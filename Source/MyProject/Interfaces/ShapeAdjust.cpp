// Fill out your copyright notice in the Description page of Project Settings.

#include "ShapeAdjust.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

#include "MyProject/Components/Asset/C_Asset.h"
#include "MyProject/DataAsset/DA_AssetBase.h"

// Add default functionality here for any IShapeAdjust functions that are not pure virtual.
void IShapeAdjust::RefreshCollision( const FName& CollisionProfileName )
{
	if ( const USceneComponent* ThisComponent = Cast<USceneComponent>(this) )
	{
		const FBoxSphereBounds Bounds = ThisComponent->GetAttachmentRoot()->GetLocalBounds();

		if ( USphereComponent* ShapeComponent = Cast<USphereComponent>( this ) )
		{
			ShapeComponent->SetSphereRadius( Bounds.SphereRadius * DefaultSizeMultiplier );
			ShapeComponent->SetCollisionProfileName( CollisionProfileName );	
		}
		else if ( UBoxComponent* BoxComponent = Cast<UBoxComponent>( this ) )
		{
			BoxComponent->SetBoxExtent( Bounds.BoxExtent * DefaultSizeMultiplier );
			BoxComponent->SetCollisionProfileName( CollisionProfileName );
		}
		else if ( UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>( this ) )
		{
			CapsuleComponent->SetCapsuleSize( Bounds.SphereRadius * DefaultSizeMultiplier , Bounds.BoxExtent.Z * DefaultSizeMultiplier );
		}
	}
	else
	{
		check( false );
	}
	
}
