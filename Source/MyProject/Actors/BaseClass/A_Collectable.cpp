// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Collectable.h"

#include "Kismet/GameplayStatics.h"
#include "MyProject/Components/C_PickUp.h"
#include "MyProject/Components/Asset/C_CollectableAsset.h"
#include "MyProject/Private/Utilities.hpp"

#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogCollectable);

const FName AA_Collectable::AssetComponentName (TEXT("AssetComponent"));
const FName AA_Collectable::RootSceneComponentName (TEXT("RootSceneComponent"));

AA_Collectable* FCollectableUtility::CloneChildActor
(
	AA_Collectable* InObject,
	const FTransform& InTransform,
	const std::function<void(AActor*)>& InDeferredFunction
)
{
	if (!InObject)
	{
		return nullptr;
	}

	UWorld* World = InObject->GetWorld();
	check(World);

	// Instigator will be nullptr if cast failed.
	APawn* Pawn = Cast<APawn>(InObject->GetOwner());
	AA_Collectable* ClonedObject = World->SpawnActorDeferred<AA_Collectable>
	(
		InObject->GetClass(),
		FTransform::Identity,
		InObject->GetOwner(),
		Pawn,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	for (UActorComponent* OriginalComponent : InObject->GetComponents())
	{
		// todo: use tag? (5.4 introduced)

		if ( OriginalComponent->IsA<USceneComponent>() )
		{
			continue;
		}

		UActorComponent* Destination = ClonedObject->FindComponentByClass
		(
			OriginalComponent->GetClass()
		);

		Destination->ReinitializeProperties(OriginalComponent);
	}
	
	ClonedObject->GetAssetComponent()->SetID(InObject->GetAssetComponent()->GetID());
	ClonedObject->FetchAsset();
	InDeferredFunction(ClonedObject);
	
	UGameplayStatics::FinishSpawningActor(ClonedObject, InTransform);
	return ClonedObject;
}

// Sets default values
AA_Collectable::AA_Collectable(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	AssetComponent = CreateDefaultSubobject<UC_CollectableAsset>(AssetComponentName);
	PickUpComponent = CreateDefaultSubobject<UC_PickUp>(TEXT("PickUpComponent"));

	// RootSceneComponent initialized by asset component;
	// set the skeletal mesh component as root component temporarily;
	SetRootComponent(SkeletalMeshComponent);

	SkeletalMeshComponent->SetSimulatePhysics(false);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	SkeletalMeshComponent->SetNetAddressable();
	SkeletalMeshComponent->SetIsReplicated(true);
	
	PickUpComponent->SetNetAddressable();
	PickUpComponent->SetIsReplicated(true);

	AssetComponent->SetNetAddressable();
	AssetComponent->SetIsReplicated(true);

	bReplicates = true;
	AActor::SetReplicateMovement(true);
	bNetLoadOnClient = true;
	bDummy = false;

	AssetComponent->OnAssetIDSet.AddUObject(this, &AA_Collectable::FetchAsset);
}

void AA_Collectable::SetDummy(const bool InFlag, AA_Collectable* InSibling)
{
	if (GetNetMode() != NM_Client)
	{
		bDummy = InFlag;
		if (InFlag)
		{
			ensure(InSibling);
		}

		AA_Collectable* Previous = Sibling;
		Sibling = InSibling;
		
		OnDummyFlagSet.Broadcast(Previous);
	}
}

void AA_Collectable::SetPhysics(const bool InPhysics)
{
	ApplyPhysics(InPhysics);
}

// Called when the game starts or when spawned
void AA_Collectable::BeginPlay()
{
	Super::BeginPlay();

}

void AA_Collectable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AA_Collectable, AssetComponent);
	DOREPLIFETIME(AA_Collectable, PickUpComponent);
	DOREPLIFETIME_CONDITION(AA_Collectable, bDummy, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AA_Collectable, Sibling, COND_OwnerOnly);
}

void AA_Collectable::PostFetchAsset()
{
	IAssetFetchable::PostFetchAsset();
	
	//BoxComponent->InitBoxExtent(SkeletalMeshComponent->GetLocalBounds().BoxExtent);
}

void AA_Collectable::OnRep_Dummy(AA_Collectable* InPreviousDummy) const
{
	OnDummyFlagSet.Broadcast(InPreviousDummy);
}

// Called every frame
void AA_Collectable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AA_Collectable::ApplyPhysics(const bool InPhysics) const
{
	LOG_FUNC_PRINTF(LogCollectable, Log, "Applying physics flag %d Client? : %d;", InPhysics, GetNetMode() == NM_Client);

	// Collectable is not initialized yet;
	check(CollisionComponent);
	
	CollisionComponent->SetSimulatePhysics(InPhysics);
	CollisionComponent->SetCollisionEnabled
	(
		InPhysics ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision
	);
}

