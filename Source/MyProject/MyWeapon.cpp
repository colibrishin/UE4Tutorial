// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWeapon.h"

#include "MyCharacter.h"
#include "MyWeaponStatComponent.h"

#include "Components/BoxComponent.h"

// Sets default values
AMyWeapon::AMyWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponStatComponent = CreateDefaultSubobject<UMyWeaponStatComponent>(TEXT("WeaponStatComponent"));
}

// Called when the game starts or when spawned
void AMyWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void AMyWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

  if (GetMesh()->GetStaticMesh() == nullptr)
  {
	  UE_LOG(LogTemp, Error, TEXT("Weapon mesh is not set"));
  }
}

bool AMyWeapon::InteractImpl(AMyCharacter* Character)
{
	if (Character->TryPickWeapon(this))
	{
		return true;
	}

	return false;
}
