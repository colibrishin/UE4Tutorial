// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCollectable.h"

#include "MyCharacter.h"

#include "Components/BoxComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AMyCollectable::AMyCollectable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));

	GetMesh()->SetupAttachment(RootComponent);
	GetCollider()->SetupAttachment(GetMesh());

	GetMesh()->SetCollisionProfileName(TEXT("MyCollectable"));
	GetCollider()->SetCollisionProfileName(TEXT("MyCollectable"));

	GetCollider()->SetBoxExtent(FVector{10.f, 30.f, 10.f});

	ItemOwner = nullptr;
	GetMesh()->SetSimulatePhysics(false);
	GetCollider()->SetSimulatePhysics(false);
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

bool AMyCollectable::Interact(class AMyCharacter* Character)
{
	if (IsBelongToCharacter())
	{
		return false;
	}

	const FVector PreviousLocation = GetActorLocation();

	//GetMesh()->SetSimulatePhysics(false);
	//GetCollider()->SetSimulatePhysics(false);

	if (GetMesh()->AttachToComponent
		(
		 Character->GetMesh(),
		 FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		 AMyCharacter::ChestSocketName
		))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("AttachToComponent success"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("AttachToComponent failed"));
		return false;
	}
	Hide();

	SetItemOwner(Character);
	return true;
}

bool AMyCollectable::Use(AMyCharacter* Character)
{
	UE_LOG(LogTemp, Warning, TEXT("Use"));
	return true;
}

void AMyCollectable::InteractInterrupted()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: InteractInterrupted"), *GetName());
}

void AMyCollectable::UseInterrupted()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: UseInterrupted"), *GetName());
}

bool AMyCollectable::Drop()
{
	if (IsBelongToCharacter())
	{
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
		//GetMesh()->SetSimulatePhysics(true);
		SetItemOwner(nullptr);
		return true;
	}

	return false;
}

void AMyCollectable::Hide() const
{
	GetMesh()->SetVisibility(false);
}

void AMyCollectable::Show() const
{
	GetMesh()->SetVisibility(true);
}

bool AMyCollectable::IsBelongToCharacter() const
{
	return ItemOwner.IsValid();
}

void AMyCollectable::SetItemOwner(AMyCharacter* NewOwner)
{
	if (ItemOwner == NewOwner)
	{
		return;
	}

	if (ItemOwner != NewOwner && ItemOwner != nullptr)
	{
		ItemOwner->UnbindOnInteractInterrupted(OnInteractInterruptedHandle);
		ItemOwner->UnbindOnUseInterrupted(OnUseInterruptedHandle);
	}

	ItemOwner = NewOwner;

	if (ItemOwner != nullptr)
	{
		OnInteractInterruptedHandle = ItemOwner->BindOnInteractInterrupted(this, &AMyCollectable::InteractInterrupted);
		OnUseInterruptedHandle = ItemOwner->BindOnUseInterrupted(this, &AMyCollectable::UseInterrupted);
	}
}

void AMyCollectable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyCollectable, ItemOwner);
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

