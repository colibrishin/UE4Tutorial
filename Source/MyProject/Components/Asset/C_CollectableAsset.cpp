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
		 TargetActor )
	{
		if (GetNetMode() != NM_Client)
		{
			const USkeletalMeshComponent* MeshComponent = TargetActor->GetComponentByClass<USkeletalMeshComponent>();
			const FBoxSphereBounds Bounds = MeshComponent->GetLocalBounds();
			const FVector CustomScale = GetAsset<UDA_Collectable>()->GetSize();
			const float ScaleDistanced = FVector::Distance( FVector::ZeroVector , CustomScale );

			if ( TargetActor->CollisionComponent )
			{
				TargetActor->CollisionComponent->UnregisterComponent();
				TargetActor->CollisionComponent->DestroyComponent();
			}
		
			switch (Collectable->GetCollisionType())
			{
			case EMultiShapeType::Box:
				{
					TargetActor->CollisionComponent = NewObject<UBoxComponent>(TargetActor, TEXT("CollisionComponent"));
					break;
				}
			case EMultiShapeType::Sphere:
				{
					TargetActor->CollisionComponent = NewObject<USphereComponent>(TargetActor, TEXT("CollisionComponent"));
					break;
				}
			case EMultiShapeType::Capsule:
				{
					TargetActor->CollisionComponent = NewObject<UCapsuleComponent>(TargetActor, TEXT("CollisionComponent"));
					break;
				}
			default: check(false);
			}

			TargetActor->CollisionComponent->SetActive( true );
			TargetActor->CollisionComponent->SetIsReplicated( true );
			TargetActor->CollisionComponent->RegisterComponent();

			USceneComponent* PreviousRootComponent = Actor->GetRootComponent();
			USceneComponent* ParentComponent = Actor->GetParentComponent();

			TargetActor->UpdateCollisionComponent( 
				PreviousRootComponent , 
				ParentComponent , 
				TargetActor->GetCollisionComponent() , 
				"MyCollectable" , 
				Collectable->GetCollisionType() , 
				Bounds , 
				Collectable->GetSize() );
		}
	}
}


// Called when the game starts
void UC_CollectableAsset::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


