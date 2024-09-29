// Fill out your copyright notice in the Description page of Project Settings.


#include "A_RangeWeapon.h"

#include "NiagaraComponent.h"

#include "MyProject/Private/Utilities.hpp"
#include "MyProject/Components/C_PickUp.h"
#include "MyProject/Components/Weapon/C_RangeWeapon.h"

const FName AA_RangeWeapon::MuzzleSocketName ( "Muzzle" ); 

// Sets default values
AA_RangeWeapon::AA_RangeWeapon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UC_RangeWeapon>(WeaponComponentName))
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BulletTrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BulletTrailComponent"));
	BulletTrailComponent->SetAutoDestroy(false);
	BulletTrailComponent->SetAutoActivate(false);
}

UC_RangeWeapon* AA_RangeWeapon::GetRangeWeaponComponent() const
{
	return Cast<UC_RangeWeapon>(GetWeaponComponent());
}

// Called when the game starts or when spawned
void AA_RangeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AA_RangeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AA_RangeWeapon::StartBulletTrailImplementation() const
{
	if (BulletTrailComponent->GetAsset())
	{
		const FVector& Normal = IsDummy() ? 
			Cast<AA_RangeWeapon>(GetSibling())->GetRangeWeaponComponent()->GetRecoiledNormal() :
			GetRangeWeaponComponent()->GetRecoiledNormal();

		LOG_FUNC_PRINTF(LogTemp, Log, "Bullet trail normal: %s, Client?: %d, Dummy?: %d", *Normal.ToString(), GetNetMode() == NM_Client, IsDummy());
		
		BulletTrailComponent->SetVariableFloat(TEXT("Yaw"), Normal.X);
		BulletTrailComponent->SetVariableFloat(TEXT("Pitch"), Normal.Y);
		BulletTrailComponent->SetVariableFloat(TEXT("Roll"), Normal.Z);
		BulletTrailComponent->Activate(true);
	}
}

void AA_RangeWeapon::PostFetchAsset()
{
	Super::PostFetchAsset();

	ensure
	(
		BulletTrailComponent->AttachToComponent(
			GetSkeletalMeshComponent(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			MuzzleSocketName)
	);
}

void AA_RangeWeapon::Attack()
{
	Super::Attack();

	if (IsDummy())
	{
		StartBulletTrailImplementation();
		return;
	}
	
	Multi_StartBulletTrail();
}

void AA_RangeWeapon::Multi_StartBulletTrail_Implementation()
{
	StartBulletTrailImplementation();
}

