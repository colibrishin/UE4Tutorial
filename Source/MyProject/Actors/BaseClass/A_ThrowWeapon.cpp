// Fill out your copyright notice in the Description page of Project Settings.


#include "A_ThrowWeapon.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ShapeComponent.h"

#include "MyProject/Components/Weapon/C_ThrowWeapon.h"

#include "Net/UnrealNetwork.h"

AA_ThrowWeapon::AA_ThrowWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UC_ThrowWeapon>(WeaponComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	
	ProjectileMovementComponent->bAutoUpdateTickRegistration = false;
	ProjectileMovementComponent->bSimulationEnabled = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->bInterpMovement = true;
	ProjectileMovementComponent->bInterpRotation = true;
}

// Called when the game starts or when spawned
void AA_ThrowWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Exclude the listen server case.
	if ( GetNetMode() == NM_Client )
	{
		// Disable the simulation. The simulation should be done by server.
		ProjectileMovementComponent->SetActive( false );
	}
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
		ProjectileMovementComponent->SetUpdatedComponent( CollisionComponent );
		CollisionComponent->SetSimulatePhysics( false );
	}
}

// Called every frame
void AA_ThrowWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

