// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCollectable.h"

#include "MyCharacter.h"

#include "Components/BoxComponent.h"

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

	GetMesh()->AttachToComponent(
		Character->GetMesh(), 
		FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	Hide();

	if (!InteractImpl(Character))
	{
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		Show();
		return false;
	}

	return false;
}

bool AMyCollectable::Drop()
{
	if (IsBelongToCharacter())
	{
		Show();
		ItemOwner = nullptr;
		return true;
	}

	return false;
}

void AMyCollectable::Hide() const
{
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetVisibility(false);
}

void AMyCollectable::Show() const
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetVisibility(true);
}

void AMyCollectable::ShowOnly() const
{
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetVisibility(true);
}

bool AMyCollectable::IsBelongToCharacter() const
{
	return ItemOwner.IsValid();
}

void AMyCollectable::SetItemOwner(AMyCharacter* FutureOwner)
{
	ItemOwner = FutureOwner;
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

