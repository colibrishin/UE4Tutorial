// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ThrowWeapon.h"

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

void UC_ThrowWeapon::Server_StopAttack_Implementation()
{
	Super::Server_StopAttack_Implementation();

	// Drop grenade first;
	Cast<IPickingUp>(GetOwner())->Drop(GetOwner()->GetComponentByClass<UC_PickUp>());
	
	const FVector Forward = GetOwner()->GetOwner()->GetActorForwardVector();
	const float CookTimeRatio = CookTimeCounter / CookingTime;
	const FVector Velocity = Forward * ThrowForce * (CookTimeRatio * ThrowForceMultiplier);

	if (USkeletalMeshComponent* SkeletalMeshComponent = GetOwner()->GetComponentByClass<USkeletalMeshComponent>())
	{
		// send it;
		SkeletalMeshComponent->AddImpulse(Velocity);	
	}

	CookTimeCounter = 0.f;
}


// Called when the game starts
void UC_ThrowWeapon::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UC_ThrowWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime , TickType , ThisTickFunction);

	if (bFiring)
	{
		CookTimeCounter += DeltaTime;
	}
}

