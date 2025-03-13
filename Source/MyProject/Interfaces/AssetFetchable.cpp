// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetFetchable.h"
#include "Components/ShapeComponent.h"

#include "MyProject/Components/Asset/C_CollectableAsset.h"

void IAssetFetchable::UpdateCollisionComponent(USceneComponent* RootComponent, USceneComponent* ParentComponent, UShapeComponent* NewComponent, const FName& CollisionProfileName)
{
	if (AActor* Actor = Cast<AActor>(this)) 
	{
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
		NewComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		NewComponent->SetAllUseCCD(true);
	}	
}

// Add default functionality here for any IAssetFetchable functions that are not pure virtual.
void IAssetFetchable::PostFetchAsset() {}
