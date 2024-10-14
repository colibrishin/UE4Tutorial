// Fill out your copyright notice in the Description page of Project Settings.


#include "C_CollectableAsset.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "MyProject/Actors/BaseClass/A_Collectable.h"
#include "MyProject/DataAsset/DA_Collectable.h"


// Sets default values for this component's properties
UC_CollectableAsset::UC_CollectableAsset()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

EMySlotType UC_CollectableAsset::GetSlot() const
{
	if (const UDA_Collectable* Collectable = GetAsset<UDA_Collectable>())
	{
		return Collectable->GetSlotType();
	}

	return EMySlotType::Unknown;
}

FString UC_CollectableAsset::GetAssetName() const
{
	if (const UDA_Collectable* Collectable = GetAsset<UDA_Collectable>())
	{
		return Collectable->GetAssetName();
	}

	return TEXT("");
}

void UC_CollectableAsset::ApplyAsset()
{
	Super::ApplyAsset();

	AActor* Actor = GetOwner();
	// cannot determine the owner, move up to the hierarchy (e.g., PreSpawn, Deferred);
	if ( !Actor )
	{
		Actor = Cast<AActor>( GetOuter() );
	}
	check( Actor != nullptr );

	const UDA_Collectable* Collectable = GetAsset<UDA_Collectable>();
	check(Collectable);
	
	if ( AA_Collectable* TargetActor = Cast<AA_Collectable>(Actor);
		TargetActor && !TargetActor->CollisionComponent )
	{
		const USkeletalMeshComponent* MeshComponent = TargetActor->GetComponentByClass<USkeletalMeshComponent>();
		const FBoxSphereBounds Bounds = MeshComponent->GetLocalBounds();
		
		switch (Collectable->GetCollisionType())
		{
		case EMultiShapeType::Box:
			{
				TargetActor->CollisionComponent = NewObject<UBoxComponent>(GetOuter(), TEXT("CollisionComponent"));
				Cast<UBoxComponent>(TargetActor->CollisionComponent)->InitBoxExtent(Bounds.BoxExtent);
				break;
			}
		case EMultiShapeType::Sphere:
			{
				TargetActor->CollisionComponent = NewObject<USphereComponent>(GetOuter(), TEXT("CollisionComponent"));
				Cast<USphereComponent>(TargetActor->CollisionComponent)->InitSphereRadius(Bounds.BoxExtent.GetMax());
				break;
			}
		case EMultiShapeType::Capsule:
			{
				// todo: accurate estimation
				TargetActor->CollisionComponent = NewObject<UCapsuleComponent>(GetOuter(), TEXT("CollisionComponent"));
				Cast<UCapsuleComponent>(TargetActor->CollisionComponent)->InitCapsuleSize
				(
					Bounds.BoxExtent.X,
					Bounds.BoxExtent.Y
				);
				break;
			}
		default: check(false);
		}
		
		TargetActor->AddOwnedComponent(TargetActor->CollisionComponent);
		// allows to replicates the client created object by the server object;
		TargetActor->CollisionComponent->SetNetAddressable();
		TargetActor->CollisionComponent->SetIsReplicated(true);

		// possible other actor's component (e.g., child actor component) 
		USceneComponent* AttachedComponent = TargetActor->GetRootComponent()->GetAttachParent();
		USceneComponent* CurrentRootComponent = TargetActor->GetRootComponent();
		USceneComponent* NewRootComponent = TargetActor->GetCollisionComponent();
		
		if (CurrentRootComponent != TargetActor->GetCollisionComponent())
		{
			CurrentRootComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			TargetActor->SetRootComponent(NewRootComponent);

			if (AttachedComponent)
			{
				LOG_FUNC_PRINTF(LogAssetComponent, Log, "Collectable was previously attached to %s;", *AttachedComponent->GetName());
				check(
					NewRootComponent->AttachToComponent
					(
						AttachedComponent,
						FAttachmentTransformRules::SnapToTargetNotIncludingScale
					));
			}

			CurrentRootComponent->AttachToComponent
			(
				NewRootComponent,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);
		}

		// lazy initialization of collision component;
		TargetActor->CollisionComponent->SetCollisionProfileName("MyCollectable");
		TargetActor->CollisionComponent->SetSimulatePhysics(false);
		TargetActor->CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		TargetActor->CollisionComponent->SetAllUseCCD(true);
	}
}


// Called when the game starts
void UC_CollectableAsset::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


