// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCollectable.h"

#include "MyCharacter.h"
#include "MyCollectableDataAsset.h"

#include "MyProject/Components/MyCollectableComponent.h"
#include "MyProject/Components/MyInventoryComponent.h"
#include "MyPlayerState.h"

#include "Components/BoxComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AMyCollectable::AMyCollectable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    CollectableComponent = CreateDefaultSubobject<UMyCollectableComponent>(TEXT("CollectableComponent"));

	SetRootComponent(Collider);
	GetCollider()->SetCollisionProfileName(TEXT("MyCollectable"));
	GetSkeletalMeshComponent()->SetCollisionProfileName(TEXT("MyCollectable"));

	GetCollider()->SetBoxExtent(FVector{10.f, 30.f, 10.f});

	SkeletalMeshComponent->SetSimulatePhysics(true);
}

// Called when the game starts or when spawned
void AMyCollectable::BeginPlay()
{
	Super::BeginPlay();
}

void AMyCollectable::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Collider->OnComponentBeginOverlap.AddDynamic(this, &AMyCollectable::OnCharacterOverlapImpl);
}

bool AMyCollectable::OnCharacterOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, AMyCharacter* Character, const FHitResult& SweepResult
)
{
	return false;
}

void AMyCollectable::Server_Drop_Implementation()
{
	if (!IsBelongToCharacter())
	{
		return;
	}

	if (const auto& MyCharacter = GetItemOwner())
	{
		MyCharacter->OnInteractInterrupted.RemoveDynamic(this, &AMyCollectable::Server_InteractInterrupted);
		MyCharacter->OnUseInterrupted.RemoveDynamic(this, &AMyCollectable::Server_UseInterrupted);
		Client_UnbindInterruption();
	}

	DropBeforeCharacter();

	SkeletalMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	DropLocation();
	Multi_Drop();
	return;
}

void AMyCollectable::Multi_Drop_Implementation()
{
	Show();
	SkeletalMeshComponent->SetSimulatePhysics(true);
}

void AMyCollectable::Server_Interact_Implementation(AMyCharacter* Character)
{
	LOG_FUNC(LogTemp, Warning, "Interact");

	if (!PreInteract(Character))
	{
		return;
	}
	if (!PostInteract(Character))
	{
		return;
	}

	//Client_Interact(Character);
	Multi_Interact(Character);

	return;
}

void AMyCollectable::Server_Use_Implementation(AMyCharacter* Character)
{
	if (!PreUse(Character))
	{
		return;
	}

	if (!PostUse(Character))
	{
		return;
	}

	//Client_Use(Character);
	//Multi_Use(Character);

	return;
}

void AMyCollectable::Server_InteractInterrupted_Implementation()
{
	LOG_FUNC(LogTemp, Warning, "InteractInterrupted");
}

void AMyCollectable::Server_UseInterrupted_Implementation()
{
	LOG_FUNC(LogTemp, Warning, "UseInterrupted");
}

void AMyCollectable::Multi_Interact_Implementation(AMyCharacter* Character)
{
	SkeletalMeshComponent->SetSimulatePhysics(false);
	Hide();
}

bool AMyCollectable::PreInteract(AMyCharacter* Character)
{
	if (IsBelongToCharacter())
	{
		LOG_FUNC(LogTemp, Warning, "Already belong to character");
		return false;
	}

	LOG_FUNC(LogTemp, Warning, "PreInteract success");
	return true;
}

bool AMyCollectable::TryAttachItem(AMyCharacter* Character)
{
	LOG_FUNC_RAW(LogTemp, Warning, *FString::Printf(TEXT("Setting Owner to : %s"), *Character->GetName()));

	if (SkeletalMeshComponent->AttachToComponent
		(
		 Character->GetMesh(),
		 FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		 AMyCharacter::ChestSocketName
		))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("AttachToComponent success"));

		if (const auto& MyCharacter = GetItemOwner())
		{
			Character->OnInteractInterrupted.AddUniqueDynamic(this, &AMyCollectable::Server_InteractInterrupted);
			Character->OnUseInterrupted.AddUniqueDynamic(this, &AMyCollectable::Server_UseInterrupted);
			Client_TryAttachItem(Character);
		}

		return true;
	}
	else
	{
		const FVector PreviousLocation = GetActorLocation();
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("AttachToComponent failed"));
		SetActorLocation(PreviousLocation);
		return false;
	}
}

bool AMyCollectable::PostInteract(AMyCharacter* Character)
{
	SkeletalMeshComponent->SetSimulatePhysics(false);

	if (TryAttachItem(Character))
	{
		LOG_FUNC(LogTemp, Warning, "PostInteract success");

		const auto& Inventory = Character->GetInventory();
		const auto& Slot = CollectableComponent->GetSlotType();
		const auto& SlotNum = static_cast<int32>(Slot);

		if (Slot == EMySlotType::Unknown)
		{
			LOG_FUNC(LogTemp, Warning, "Slot is unknown");
			return false;
		}

		LOG_FUNC_PRINTF(LogTemp, Warning, "SlotNum: %d", SlotNum);

		if (Inventory->TryAddItem(this, SlotNum))
		{
			LOG_FUNC_PRINTF(LogTemp, Warning, "Server-side Item Interacted: %s", *GetName());
		}
		else 
		{
			LOG_FUNC(LogTemp, Warning, "Server-side Item Interacted failed");
			return false;
		}

		if (const auto& TargetPlayerState = Character->GetPlayerState<AMyPlayerState>())
		{
			if (!IsValid(TargetPlayerState->GetHand()))
			{
				TargetPlayerState->SetCurrentItem(this);
			}
		}

		return true;
	}
	else
	{
		LOG_FUNC(LogTemp, Warning, "PostInteract failed");
		Server_Drop();
		return false;
	}
}

void AMyCollectable::Client_TryAttachItem_Implementation(AMyCharacter* Character)
{
	Character->OnInteractInterrupted.AddUniqueDynamic(this, &AMyCollectable::Server_InteractInterrupted);
	Character->OnUseInterrupted.AddUniqueDynamic(this, &AMyCollectable::Server_UseInterrupted);
}

void AMyCollectable::Client_UnbindInterruption_Implementation()
{
	const auto& MyCharacter = GetItemOwner();
	MyCharacter->OnInteractInterrupted.RemoveDynamic(this, &AMyCollectable::Server_InteractInterrupted);
	MyCharacter->OnUseInterrupted.RemoveDynamic(this, &AMyCollectable::Server_UseInterrupted);
}

bool AMyCollectable::PreUse(AMyCharacter* Character)
{
	LOG_FUNC(LogTemp, Warning, "PreUse");
	return true;
}

bool AMyCollectable::PostUse(AMyCharacter* Character)
{
	LOG_FUNC(LogTemp, Warning, "PostUse");
	return true;
}

void AMyCollectable::DropBeforeCharacter()
{
	const auto& MyCharacter = GetItemOwner();
	MyCharacter->GetInventory()->Remove(this);
}


void AMyCollectable::DropLocation()
{
	const FVector PreviousLocation = GetActorLocation();
	FHitResult HitResult;
	FCollisionQueryParams Params {NAME_None, false, this};

	const auto& Result = GetWorld()->LineTraceSingleByProfile
	(
		HitResult,
		PreviousLocation,
		PreviousLocation - FVector::UpVector * 1000.f,
		TEXT("IgnoreOnlyPawn"),
		Params
	);

	if (Result)
	{
		UE_LOG(LogTemp, Warning, TEXT("Drop success"));
		UE_LOG(LogTemp, Warning, TEXT("HitResult.Location: %s"), *HitResult.ImpactPoint.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
		SetActorLocation(HitResult.Location);
		SetActorRotation(FRotator::ZeroRotator);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Drop Fallbacked"));
		UE_LOG(LogTemp, Warning, TEXT("PreviousLocation: %s"), *PreviousLocation.ToString());
		SetActorLocation(PreviousLocation);
		SetActorRotation(FRotator::ZeroRotator);
	}
}

void AMyCollectable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyCollectable, CollectableComponent);
}

AMyCharacter* AMyCollectable::GetItemOwner() const
{
	const auto& CollectableOwner = GetAttachParentActor();
	return Cast<AMyCharacter>(CollectableOwner);
}

void AMyCollectable::Hide() const
{
	SkeletalMeshComponent->SetVisibility(false);
}

void AMyCollectable::Show() const
{
	SkeletalMeshComponent->SetVisibility(true);
}

void AMyCollectable::UpdateAsset()
{
	LOG_FUNC_PRINTF(LogTemp, Log, "Updating Asset for collectable %s", *GetName());
	GetCollectableComponent()->ApplyAsset();
}

bool AMyCollectable::IsBelongToCharacter() const
{
	return IsValid(GetAttachParentActor());
}

// Called every frame
void AMyCollectable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyCollectable::OnCharacterOverlapImpl(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult
)
{
	if (IsBelongToCharacter())
	{
		return;
	}

	const auto Character = Cast<AMyCharacter>(OtherActor);

	if (IsValid(Character))
	{
		OnCharacterOverlap(OverlappedComponent , OtherActor , OtherComp , OtherBodyIndex , bFromSweep , Character, SweepResult);
	}
}


