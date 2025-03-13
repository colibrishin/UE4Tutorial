// Fill out your copyright notice in the Description page of Project Settings.


#include "A_ThrowWeapon.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ShapeComponent.h"

#include "MyProject/Components/Weapon/C_ThrowWeapon.h"

#include "Net/UnrealNetwork.h"

AA_ThrowWeapon::AA_ThrowWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UC_ThrowWeapon>(WeaponComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	
	// Disable the simulation by default. The simulation should be done by server.
	ProjectileMovementComponent->bSimulationEnabled = false;
	ProjectileMovementComponent->bShouldBounce = true;

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
}

void AA_ThrowWeapon::OnRep_CollisionComponent()
{
	Super::OnRep_CollisionComponent();

	if ( GetCollisionComponent() )
	{
		// Update projectile movement to collision component
		ProjectileMovementComponent->SetUpdatedComponent( GetRootComponent() );
		CollisionComponent->SetSimulatePhysics( false );
	}
}

// Called every frame
void AA_ThrowWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

