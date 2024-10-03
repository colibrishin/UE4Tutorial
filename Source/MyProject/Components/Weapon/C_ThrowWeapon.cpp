// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ThrowWeapon.h"

#include "MyProject/Actors/BaseClass/A_Character.h"
#include "MyProject/Actors/BaseClass/A_Collectable.h"
#include "MyProject/Interfaces/PickingUp.h"


// Sets default values for this component's properties
UC_ThrowWeapon::UC_ThrowWeapon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	bCanSpray = false;
}

// Called when the game starts
void UC_ThrowWeapon::BeginPlay()
{
	Super::BeginPlay();

	// ...

	OnStopAttack.AddUniqueDynamic(this, &UC_ThrowWeapon::Throw);

	// use the OnObjectDropSpawned instead of OnStopAttack;
	// new object will be created while in drop, throwing force and setup need to be applied to the new object
	// note that at OnObjectDropSpawned point, this object is not destroyed yet;
	UC_PickUp* PickUpComponent = GetOwner()->GetComponentByClass<UC_PickUp>();
	// ensure that pickup component is available;
	check(PickUpComponent);
	PickUpComponent->OnObjectDropSpawned.AddUniqueDynamic(this, &UC_ThrowWeapon::SetUpSpawnedObject);
	
}

void UC_ThrowWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime , TickType , ThisTickFunction);

	if (bFiring)
	{
		CookTimeCounter += DeltaTime;
	}
}

void UC_ThrowWeapon::SetUpSpawnedObject(AActor* InSpawnedActor)
{
	const AActor* WeaponOwner = GetOwner()->GetAttachParentActor();
	const FVector Forward = WeaponOwner->GetActorForwardVector();
	const float CookTimeRatio = CookTimeCounter / CookingTime;
	const FVector Velocity = Forward * ThrowForce * (CookTimeRatio * ThrowForceMultiplier);

	if (USkeletalMeshComponent* SkeletalMeshComponent = InSpawnedActor->GetComponentByClass<USkeletalMeshComponent>())
	{
		// send it;
		SkeletalMeshComponent->AddImpulse(Velocity);
	}

	CookTimeCounter = 0.f;
}

void UC_ThrowWeapon::Throw(UC_Weapon* InWeapon)
{
	// use the drop weapon sequence;
	AA_Character*    OwningActor     = Cast<AA_Character>(GetOwner()->GetAttachParentActor());
	const UC_PickUp* PickUpComponent = GetOwner()->GetComponentByClass<UC_PickUp>();
	check(PickUpComponent);
	PickUpComponent->OnObjectDrop.Broadcast(OwningActor, true);
}

