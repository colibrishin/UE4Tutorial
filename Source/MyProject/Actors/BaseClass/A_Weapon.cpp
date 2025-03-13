// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Weapon.h"

#include "MyProject/Components/Asset/C_WeaponAsset.h"
#include "MyProject/Components/Weapon/C_Weapon.h"

#include "Net/UnrealNetwork.h"

const FName AA_Weapon::WeaponComponentName( "WeaponComponent" );

AA_Weapon::AA_Weapon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UC_WeaponAsset>(AssetComponentName))
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponComponent = CreateDefaultSubobject<UC_Weapon>(WeaponComponentName);
	WeaponComponent->SetNetAddressable();
	WeaponComponent->SetIsReplicated(true);
}

UC_Weapon* AA_Weapon::GetWeaponComponent() const
{
	return WeaponComponent;
}

// Called when the game starts or when spawned
void AA_Weapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AA_Weapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AA_Weapon, WeaponComponent);
}

// Called every frame
void AA_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AA_Weapon::Attack() {}

void AA_Weapon::Reload() {}

