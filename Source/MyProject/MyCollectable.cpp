// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCollectable.h"

#include "MyCharacter.h"
#include "MyInventoryComponent.h"
#include "MyPlayerState.h"

#include "Components/BoxComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AMyCollectable::AMyCollectable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));

	GetCollider()->SetCollisionProfileName(TEXT("MyCollectable"));

	GetCollider()->SetBoxExtent(FVector{10.f, 30.f, 10.f});

	StaticMeshComponent->SetSimulatePhysics(true);
	SkeletalMeshComponent->SetSimulatePhysics(true);

	SetStaticMesh();
}

// Called when the game starts or when spawned
void AMyCollectable::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(true);
    SetReplicateMovement(true);
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

bool AMyCollectable::TryAttachItem(const AMyCharacter* Character)
{
	LOG_FUNC_RAW(LogTemp, Warning, *FString::Printf(TEXT("Setting Owner to : %s"), *Character->GetName()));

	if (GetMesh()->AttachToComponent
		(
		 Character->GetMesh(),
		 FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		 AMyCharacter::ChestSocketName
		))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("AttachToComponent success"));

		const auto& MyCharacter = GetItemOwner();

		OnInteractInterruptedHandle = MyCharacter->BindOnInteractInterrupted(this, &AMyCollectable::InteractInterrupted);
		OnUseInterruptedHandle = MyCharacter->BindOnUseInterrupted(this, &AMyCollectable::UseInterrupted);
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
	GetMesh()->SetSimulatePhysics(false);

	if (TryAttachItem(Character))
	{
		LOG_FUNC(LogTemp, Warning, "PostInteract success");
		Hide();

		if (HasAuthority())
		{
			const auto& Inventory = Character->GetInventory();

			if (Inventory->TryAddItem(this))
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
				if (!IsValid(TargetPlayerState->GetCurrentHand()))
				{
					TargetPlayerState->SetCurrentItem(this);
				}
			}
		}

		return true;
	}
	else
	{
		LOG_FUNC(LogTemp, Warning, "PostInteract failed");
		Drop();
		return false;
	}
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

void AMyCollectable::DropImpl()
{
}

AMyCharacter* AMyCollectable::GetItemOwner() const
{
	const auto& CollectableOwner = GetAttachParentActor();
	return Cast<AMyCharacter>(CollectableOwner);
}

void AMyCollectable::InteractImpl(class AMyCharacter* Character)
{
	Super::InteractImpl(Character);

	LOG_FUNC(LogTemp, Warning, "Interact");

	if (!PreInteract(Character))
	{
		return;
	}
	if (!PostInteract(Character))
	{
		return;
	}

	return;
}

void AMyCollectable::UseImpl(AMyCharacter* Character)
{
	Super::UseImpl(Character);

	if (!PreUse(Character))
	{
		return;
	}

	if (!PostUse(Character))
	{
		return;
	}

	return;
}

void AMyCollectable::InteractInterruptedImpl()
{
	Super::InteractInterruptedImpl();

	LOG_FUNC(LogTemp, Warning, "InteractInterrupted");
}

void AMyCollectable::UseInterruptedImpl()
{
	Super::UseInterruptedImpl();

	LOG_FUNC(LogTemp, Warning, "UseInterrupted");
}

bool AMyCollectable::Drop()
{
	if (!IsBelongToCharacter())
	{
		return false;
	}

	const auto& MyCharacter = GetItemOwner();
	MyCharacter->UnbindOnInteractInterrupted(OnInteractInterruptedHandle);
	MyCharacter->UnbindOnUseInterrupted(OnUseInterruptedHandle);

	FVector PreviousLocation = GetActorLocation();

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

	DropImpl();

	GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

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

	Show();
	GetMesh()->SetSimulatePhysics(true);
	return true;
}

void AMyCollectable::Hide() const
{
	GetMesh()->SetVisibility(false);
}

void AMyCollectable::Show() const
{
	GetMesh()->SetVisibility(true);
}

void AMyCollectable::SetSkeletalMesh()
{
	if (MeshComponent.IsValid())
	{
		MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}

	MeshComponent = SkeletalMeshComponent;

	SetRootComponent(MeshComponent.Get());
	GetCollider()->SetupAttachment(MeshComponent.Get());
}

void AMyCollectable::SetStaticMesh()
{
	if (MeshComponent.IsValid())
	{
		MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}

	MeshComponent = StaticMeshComponent;

	SetRootComponent(MeshComponent.Get());
	GetCollider()->SetupAttachment(MeshComponent.Get());
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

