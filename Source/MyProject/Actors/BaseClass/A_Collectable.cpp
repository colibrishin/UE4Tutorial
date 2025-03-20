// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Collectable.h"

#include "Components/BoxComponent.h"

#include "Kismet/GameplayStatics.h"
#include "MyProject/Components/C_PickUp.h"
#include "MyProject/Components/Asset/C_CollectableAsset.h"
#include "MyProject/Private/Utilities.hpp"
#include "MyProject/DataAsset/DA_Collectable.h"

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

	for ( UActorComponent* OriginalComponent : InObject->GetComponents() )
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

		Destination->ReinitializeProperties( OriginalComponent );
		// Register the component for the any tick components.
		Destination->RegisterComponent();
	}

	// fixme: inconsistent fetching (delegation);
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

	CollisionComponent = CreateOptionalDefaultSubobject<UShapeComponent>(TEXT("CollisionComponent"));
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	AssetComponent = CreateDefaultSubobject<UC_CollectableAsset>(AssetComponentName);
	PickUpComponent = CreateDefaultSubobject<UC_PickUp>(TEXT("PickUpComponent"));

	SetRootComponent( SkeletalMeshComponent );
	SkeletalMeshComponent->SetSimulatePhysics(false);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickUpComponent->SetNetAddressable();
	PickUpComponent->SetIsReplicated(true);

	AssetComponent->SetNetAddressable();
	AssetComponent->SetIsReplicated(true);
	
	bReplicates = true;
	AActor::SetReplicateMovement(true);
	bNetLoadOnClient = true;
	bDummy = false;

	bPhysicsInClient = false;
	CollisionTypeInClient = ECollisionEnabled::QueryAndProbe;

	AssetComponent->OnAssetIDSet.AddUObject( this , &AA_Collectable::FetchAsset );
}

void AA_Collectable::SetDummy(const bool InFlag, AA_Collectable* InSibling)
{
	if ( HasAuthority() )
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

void AA_Collectable::SetPhysicsInClient(const bool InPhysics)
{
	bPhysicsInClient = InPhysics;
}

void AA_Collectable::SetCollisionTypeInClient( const ECollisionEnabled::Type InType )
{
	CollisionTypeInClient = InType;
}

// Called when the game starts or when spawned
void AA_Collectable::BeginPlay()
{
	Super::BeginPlay();
}

void AA_Collectable::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );
	DOREPLIFETIME( AA_Collectable , CollisionComponent );
	DOREPLIFETIME( AA_Collectable , AssetComponent );
	DOREPLIFETIME( AA_Collectable , PickUpComponent );
	DOREPLIFETIME( AA_Collectable , bPhysicsInClient );
	DOREPLIFETIME( AA_Collectable , CollisionTypeInClient );
	DOREPLIFETIME_CONDITION( AA_Collectable , bDummy , COND_OwnerOnly );
	DOREPLIFETIME_CONDITION( AA_Collectable , Sibling , COND_OwnerOnly );
}

void AA_Collectable::PostFetchAsset()
{
	IAssetFetchable::PostFetchAsset();

	if ( PickUpComponent )
	{
		PickUpComponent->AttachToComponent( SkeletalMeshComponent , FAttachmentTransformRules::SnapToTargetNotIncludingScale );
		PickUpComponent->RefreshCollision( "MyCollectable" );
	}
}

void AA_Collectable::PostNetInit()
{
	Super::PostNetInit();

	if ( PickUpComponent )
	{
		PickUpComponent->RefreshCollision( "MyCollectable" );
	}
}

void AA_Collectable::OnRep_Dummy(AA_Collectable* InPreviousDummy) const
{
	OnDummyFlagSet.Broadcast(InPreviousDummy);
}

void AA_Collectable::OnRep_PhysicsInClient() const
{
	// Exclude the listen server case.
	if ( GetNetMode() == NM_Client && CollisionComponent )
	{
		LOG_FUNC_PRINTF( LogCollectable , Log , "Applying physics flag %d and %s Client? : %d;" , bPhysicsInClient);
		CollisionComponent->SetSimulatePhysics( bPhysicsInClient );
	}	
}

void AA_Collectable::OnRep_CollisionTypeInClient() const
{
	// Exclude the listen server case.
	if ( GetNetMode() == NM_Client && CollisionComponent )
	{
		LOG_FUNC_PRINTF( LogCollectable , Log , "Applying the Collision type %s in client" , *EnumToString( CollisionTypeInClient.GetValue() ) );
		CollisionComponent->SetCollisionEnabled( CollisionTypeInClient );
	}
}

void AA_Collectable::OnRep_CollisionComponent()
{
	if ( const UDA_Collectable* Asset = GetAssetComponent()->GetAsset<UDA_Collectable>() )
	{
		UpdateCollisionComponent( 
			GetRootComponent() , 
			GetParentComponent() , 
			GetCollisionComponent() , 
			"MyCollectable" , 
			Asset->GetCollisionType() , 
			RootComponent->GetLocalBounds() , 
			Asset->GetSize() );

		if ( PickUpComponent )
		{
			PickUpComponent->RefreshCollision( "MyCollectable" );
		}
	}
}

// Called every frame
void AA_Collectable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


