// Fill out your copyright notice in the Description page of Project Settings.


#include "A_RangeWeapon.h"

#include "NiagaraComponent.h"

#include "MyProject/Components/C_PickUp.h"
#include "MyProject/Components/Weapon/C_RangeWeapon.h"

#include "Net/UnrealNetwork.h"

const FName AA_RangeWeapon::MuzzleSocketName ( "Muzzle" ); 

// Sets default values
AA_RangeWeapon::AA_RangeWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Destroy previously defined component; using range weapon component for normal and CDO case (AA_Weapon)
	WeaponComponent->DestroyComponent();

	WeaponComponent = CreateDefaultSubobject<UC_RangeWeapon>(TEXT("RangeWepaonComponent"));
	WeaponComponent->SetNetAddressable();
	WeaponComponent->SetIsReplicated(true);

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
	DOREPLIFETIME(AA_RangeWeapon, WeaponComponent);
}

// Called every frame
void AA_RangeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AA_RangeWeapon::StartBulletTrailImplementation() const
{
	if (BulletTrailComponent->GetAsset())
	{
		const FVector& Normal = IsDummy() ? 
			Cast<AA_RangeWeapon>(GetSibling())->GetRangeWeaponComponent()->GetRecoiledNormal() :
			GetRangeWeaponComponent()->GetRecoiledNormal();
		
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
			GetPickUpComponent(),
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

