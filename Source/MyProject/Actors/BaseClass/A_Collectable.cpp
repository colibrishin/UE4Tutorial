// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Collectable.h"

#include "MyProject/Components/C_PickUp.h"
#include "MyProject/Components/Asset/C_CollectableAsset.h"

#include "Net/UnrealNetwork.h"

const FName AA_Collectable::AssetComponentName (TEXT("AssetComponent"));

AA_Collectable* FCollectableUtility::CloneChildActor(
	AA_Collectable* InObject, const std::function<void(AActor*)>& InDeferredFunction
)
{
	if (!InObject)
	{
		return nullptr;
	}

	UWorld* World = InObject->GetWorld();
	check(World);

	const auto& PreSpawn = [&InObject, &InDeferredFunction](AActor* InActor)
	{
		const AA_Collectable* GeneratedObject = Cast<AA_Collectable>(InActor);
		
		for (UActorComponent* OriginalComponent : InObject->GetComponents())
		{
			// todo: use tag? (5.4 introduced)
			UActorComponent* Destination = InActor->FindComponentByClass
			(
				OriginalComponent->GetClass()
			);

			Destination->ReinitializeProperties(OriginalComponent);
		}

		InDeferredFunction(InActor);
		GeneratedObject->GetAssetComponent()->SetID(InObject->GetAssetComponent()->GetID());
	};
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.CustomPreSpawnInitalization = PreSpawn;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.bNoFail = true;
	
	AA_Collectable* GeneratedObject = World->SpawnActor<AA_Collectable>(
		InObject->GetClass(),
		SpawnParameters);
		
	return GeneratedObject;
}

// Sets default values
AA_Collectable::AA_Collectable(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	AssetComponent = CreateDefaultSubobject<UC_CollectableAsset>(TEXT("AssetComponent"));
	PickUpComponent = CreateDefaultSubobject<UC_PickUp>(TEXT("PickUpComponent"));

	SetRootComponent(SkeletalMeshComponent);

	PickUpComponent->SetNetAddressable();
	PickUpComponent->SetIsReplicated(true);

	AssetComponent->SetNetAddressable();
	AssetComponent->SetIsReplicated(true);

	AssetComponent->OnAssetIDSet.AddUObject(
		this, &AA_Collectable::FetchAsset);

	SkeletalMeshComponent->SetCollisionProfileName("MyCollectable");

	bReplicates = true;
	AActor::SetReplicateMovement(true);
	bNetLoadOnClient = true;
	bDummy = false;
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
		
		// Disable pickup component;
		PickUpComponent->SetActive(!InFlag);
		OnDummyFlagSet.Broadcast(Previous);
	}
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

void AA_Collectable::OnRep_Dummy(AA_Collectable* InPreviousDummy) const
{
	OnDummyFlagSet.Broadcast(InPreviousDummy);
}

// Called every frame
void AA_Collectable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

