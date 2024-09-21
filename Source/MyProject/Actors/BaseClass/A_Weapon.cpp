// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Weapon.h"

#include "NiagaraComponent.h"

#include "MyProject/Components/Asset/C_WeaponAsset.h"
#include "MyProject/Components/Weapon/C_Weapon.h"

#include "Net/UnrealNetwork.h"


// Sets default values
AA_Weapon::AA_Weapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AssetComponent = CreateDefaultSubobject<UC_WeaponAsset>(TEXT("WeaponAsset"));
	WeaponComponent = CreateDefaultSubobject<UC_Weapon>(TEXT("WeaponCompoent"));
	BulletTrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BulletTrailComponent"));

	AssetComponent->SetNetAddressable();
	WeaponComponent->SetNetAddressable();

	AssetComponent->SetIsReplicated(true);
	WeaponComponent->SetIsReplicated(true);

	BulletTrailComponent->SetAutoDestroy(false);
	BulletTrailComponent->SetAutoActivate(false);
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

void AA_Weapon::Multi_StartBulletTrail_Implementation()
{
	if (WeaponComponent->GetWeaponType() == EMyWeaponType::Range &&
		BulletTrailComponent->GetAsset())
	{
		BulletTrailComponent->Activate(true);
	}
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

void AA_Weapon::Attack()
{
	Multi_StartBulletTrail();	
}

void AA_Weapon::Reload()
{
	
}

