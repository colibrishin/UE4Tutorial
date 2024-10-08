// Fill out your copyright notice in the Description page of Project Settings.


#include "A_ThrowWeapon.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "MyProject/Components/Weapon/C_ThrowWeapon.h"

AA_ThrowWeapon::AA_ThrowWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UC_ThrowWeapon>(WeaponComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	
	// Update projectile movement to mesh component
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);

	// do not simulate yet;
	ProjectileMovementComponent->bSimulationEnabled = false;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;

	ProjectileMovementComponent->bInterpMovement = true;
	ProjectileMovementComponent->bInterpRotation = true;
}

// Called when the game starts or when spawned
void AA_ThrowWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AA_ThrowWeapon::PostFetchAsset()
{
	Super::PostFetchAsset();

	//SphereComponent->SetSphereRadius(SkeletalMeshComponent->GetLocalBounds().BoxExtent.GetMax());
}

// Called every frame
void AA_ThrowWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

