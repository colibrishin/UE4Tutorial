// Fill out your copyright notice in the Description page of Project Settings.


#include "A_ThrowWeapon.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "MyProject/Components/Weapon/C_ThrowWeapon.h"

#include "Net/UnrealNetwork.h"

AA_ThrowWeapon::AA_ThrowWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UC_ThrowWeapon>(WeaponComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	// Update projectile movement to mesh component
	ProjectileMovementComponent->SetUpdatedComponent(SkeletalMeshComponent);

	// do not simulate yet;
	ProjectileMovementComponent->bSimulationEnabled = false;
	ProjectileMovementComponent->bShouldBounce = true;
}

// Called when the game starts or when spawned
void AA_ThrowWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AA_ThrowWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

