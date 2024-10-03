// Fill out your copyright notice in the Description page of Project Settings.


#include "A_ThrowWeapon.h"

#include "MyProject/Components/Weapon/C_ThrowWeapon.h"

AA_ThrowWeapon::AA_ThrowWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UC_ThrowWeapon>(WeaponComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	GetSkeletalMeshComponent()->SetAngularDamping(0.8f);
	GetSkeletalMeshComponent()->SetLinearDamping(0.5f);
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

