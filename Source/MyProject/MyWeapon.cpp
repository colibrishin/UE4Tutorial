// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWeapon.h"

#include "MyCharacter.h"

#include "Components/BoxComponent.h"

// Sets default values
AMyWeapon::AMyWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Spline(TEXT("StaticMesh'/Game/ParagonBoris/FX/Meshes/Splines/SM_Forward_Burst_Splines.SM_Forward_Burst_Splines'"));

	if (SM_Spline.Succeeded())
	{
		GetMesh()->SetStaticMesh(SM_Spline.Object);
	}
}

// Called when the game starts or when spawned
void AMyWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

bool AMyWeapon::OnCharacterOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool                 bFromSweep, AMyCharacter* Character, const FHitResult& SweepResult
)
{
	return Character->TryPickWeapon(this);
}

